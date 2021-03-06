//export GST_DEBUG="alsa*:6"

/*

 gst-launch-1.0 pulsesrc device="alsa_input.usb-HD_Camera_Manufacturer_HD_
 USB_Camera_SN0008-03-Camera.analog-mono" ! audiorate ! faac perfect-timestamp=1
 hard-resync=1 bitrate=64000 ! progressreport name=audioprogress ! queue !
 flvmux name=m ! progressreport name=muxprogress ! tee name=t ! queue !
 progressreport name=local ! filesink location=mux.flv t. ! queue leaky=2 !
 progressreport name=liveprogress ! rtmpsink location=rtmp://103.226.217.136:1935/live/myStream
 async=1 v4l2src device=/dev/video2 ! video/x-h264, framerate=5/1, width=640, height=480 !
 h264parse ! progressreport name=videoprogress ! queue ! m.

 gst-launch-1.0 pulsesrc device="alsa_input.usb-HD_Camera_Manufacturer_HD_
 USB_Camera_SN0008-03-Camera.analog-mono" ! audiorate ! progressreport
 name=audioprogress ! queue ! flvmux ! progressreport name=muxprogress !
 queue leaky=2 ! progressreport name=liveprogress ! rtmpsink
 location=rtmp://103.226.217.136:1935/live/myStream async=1

 gst-launch-1.0 -v v4l2src device=/dev/video1 ! h264parse ! mux.
 alsasrc buffer-time=99999999999999 device=hw:CARD=Camera,DEV=0 do-timestamp=false !
 audio/x-raw,width=16,depth=16,rate=44100,channels=1 ! queue leaky=1 ! mux. flvmux name=mux !
 progressreport ! filesink location=test.flv

 */

#include <gst/gst.h>
#include <json/json.h>
#include <glib.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <../TCPClient/Client.h>

GMainLoop *loop;
GstElement *pipeline, *bin, *vSource, *qCam, *filter, *flvmux, *qLive, *tee,
		*qLocal, *parse, *x264enc, *sinkLocal, *sinkLive, *lclProgres,
		*liveProgres, *muxProgres, *aSource, *qAudio;

GstPad *tee_q1_pad, *tee_q2_pad, *q1_pad, *q2_pad;

GstBus *bus;
guint bus_watch_id;
GstCaps *vCap, *aCap;
GstPad *pad;

gboolean isH264;
char timeStr[20];

void getCurTime() {
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeStr, 16, "%d%m%y_%H%M%S", timeinfo);
}

static void queue_overrun(GstElement *queue, gpointer data) {
	g_print("*** Overrun Caught ***\n");
	guint mBuf, mByt, mTim, cBuf, cByt, cTim;

	g_object_get(G_OBJECT (queue), "max-size-time", &mBuf, NULL);
	g_object_get(G_OBJECT (queue), "max-size-bytes", &mByt, NULL);
	g_object_get(G_OBJECT (queue), "max-size-time", &mTim, NULL);
	g_object_get(G_OBJECT (queue), "current-level-time", &cBuf, NULL);
	g_object_get(G_OBJECT (queue), "current-level-bytes", &cByt, NULL);
	g_object_get(G_OBJECT (queue), "current-level-time", &cTim, NULL);

	g_print("Status: %u, %u, %u, %u, %u, %u\n", mBuf, cBuf, mByt, cByt, mTim,
			cTim);
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {

	switch (GST_MESSAGE_TYPE(msg)) {

	case GST_MESSAGE_EOS:
		g_print("*** End of stream ***\n");
		stopStreaming();
		break;

	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *error;

		gst_message_parse_error(msg, &error, &debug);
		g_printerr("*** ERROR: %s ***\n", error->message);
		g_error_free(error);
		g_free(debug);
		stopStreaming();
		break;
	}

	case GST_MESSAGE_INFO: {
		gchar *debug;
		GError *error;

		gst_message_parse_info(msg, &error, &debug);
		g_free(debug);

		g_print("*** INFO: %s ***\n", error->message);
		g_error_free(error);

		//stopStreaming();
		break;
	}

	default:
		break;
	}

	return TRUE;
}

void setCamera() {

	char *cam = (gchar *) getJsonValueFromFile("h264device");
	FILE *file;
	if ((file = fopen(cam, "r"))) {
		fclose(file);
		isH264 = TRUE;
		g_print("*** Using H264 camera ***\n");
		return;
	}
	g_print("*** Using RAW/JPEG camera ***\n");
	isH264 = FALSE;
}

int initCamera() {
	setCamera();

	vSource = gst_element_factory_make("v4l2src", "h264-vSource");
	qCam = gst_element_factory_make("queue", "queue-camera");
	if (isH264)
		vCap = gst_caps_from_string((gchar *) getJsonValueFromFile("h264caps"));
	else {
		vCap = gst_caps_from_string((gchar *) getJsonValueFromFile("v4l2caps"));
		x264enc = gst_element_factory_make("x264enc", "x264-enc");
	}
	parse = gst_element_factory_make("h264parse", "h264-parse");
	if (!vSource || !qCam || !parse) {
		g_printerr(
				"*** One element from initialize Camera could not be created. Exiting ***\n");
		g_printerr("*** %d, %d, %d ***\n", vSource, qCam, parse);

		return -1;
	}
	/* we set the input Camera device to the vSource element */
	if (isH264)
		g_object_set(G_OBJECT(vSource), "device",
				(gchar *) getJsonValueFromFile("h264device"), NULL);
	else {
		g_object_set(G_OBJECT(vSource), "device",
				(gchar *) getJsonValueFromFile("v4l2device"), NULL);

		/* we set the x264enc property speed-preset to ultrafast */
		g_object_set(G_OBJECT(x264enc), "speed-preset", 1, NULL);
	}

	/* we add all elements into the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), vSource, qCam, parse, NULL);

	if (!isH264)
		gst_bin_add_many(GST_BIN(pipeline), x264enc, NULL);

	/* we link the elements together */
	gst_element_link_filtered((GstElement*) vSource, qCam, vCap);
	if (!isH264)
		gst_element_link_many(qCam, x264enc, parse, flvmux, NULL);
	else
		gst_element_link_many(qCam, parse, flvmux, NULL);
	return 0;

}

int initAudio() {
	aSource = gst_element_factory_make("alsasrc", "alsa-aSource");
	qAudio = gst_element_factory_make("queue", "queue-audio");
	aCap = gst_caps_from_string((gchar *) getJsonValueFromFile("audiocaps"));
	if (!aSource || !qAudio) {
		g_printerr(
				"*** One element from initialize Audio could not be created. Exiting ***\n");
		return -1;
	}
	/* we set the input Pulse Audio device to the aSource element */
	g_object_set(G_OBJECT(aSource), "device",
			(gchar *) getJsonValueFromFile("audiodevice"), NULL);
	/* we set the input Pulse Audio device to the aSource element */
	g_object_set(G_OBJECT(aSource), "buffer-time", 99999999999999, NULL);
	/* we set the input Pulse Audio device to the aSource element */
	g_object_set(G_OBJECT(aSource), "do-timestamp", TRUE, NULL);
	/* we set the output to PAUSE async to filesink */
	g_object_set(G_OBJECT(qAudio), "leaky", 1, NULL);
	/* we set the live progress report frequency to 2 seconds */
	g_object_set(G_OBJECT(qAudio), "max-size-buffers", 0, NULL); //50, 200
	g_object_set(G_OBJECT(qAudio), "max-size-bytes", 0, NULL); //1000000000
	g_object_set(G_OBJECT(qAudio), "max-size-time", 3000000000, NULL); //3000000000
	/* we add all elements into the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), aSource, qAudio, NULL);

	gst_element_link_filtered((GstElement*) aSource, qAudio, aCap);
	gst_element_link_many(qAudio, flvmux, NULL);
	return 0;

}

int initLocal() {
	qLocal = gst_element_factory_make("queue", "queue-local");
	sinkLocal = gst_element_factory_make("filesink", "local-output");
	lclProgres = gst_element_factory_make("progressreport", "Local-Progress");
	if (!qLocal || !sinkLocal || !lclProgres) {
		g_printerr(
				"*** One element from initialize Local Sink could not be created. Exiting ***\n");
		return -1;
	}

	/* we set the output local filename to the sink element */
	getCurTime();
	char loc[128];
	sprintf(timeStr, "%s.flv", timeStr);
	sprintf(loc, "%s%s", (gchar *) getJsonValueFromFile("location"), timeStr);
	g_object_set(G_OBJECT(sinkLocal), "location", loc, NULL);
	g_object_set(G_OBJECT(sinkLocal), "enable-last-sample", FALSE, NULL);
	/* we set the local progress report frequency to 2 seconds */
	g_object_set(lclProgres, "update-freq", 2, NULL);
	/* Obtaining request pads for the tee elements*/
	GstPadTemplate *tee_src_pad_template;
	if (!(tee_src_pad_template = gst_element_class_get_pad_template(
			GST_ELEMENT_GET_CLASS(tee), "src_%u"))) {
		gst_object_unref(pipeline);
		g_critical("*** Unable to get pad template ***");
		return -1;
	}
	/* Manually link the Tee, which has "Request" pads */
	tee_q1_pad = gst_element_request_pad(tee, tee_src_pad_template, NULL, NULL);
	q1_pad = gst_element_get_static_pad(qLocal, "sink");
	/* we add all elements into the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), qLocal, lclProgres, sinkLocal, NULL);
	/* Link the tee to the queue 1 */
	if (gst_pad_link(tee_q1_pad, q1_pad) != GST_PAD_LINK_OK) {
		g_critical("*** Tee for q1 could not be linked ***\n");
		gst_object_unref(pipeline);
		return -1;
	}
	gst_object_unref(q1_pad);
	/* we link the elements together */
	gst_element_link_many(qLocal, lclProgres, sinkLocal, NULL);
	return 0;
}

int initLive() {
	bin = gst_bin_new("liveBin");
	qLive = gst_element_factory_make("queue", "queue-live");
	sinkLive = gst_element_factory_make("rtmpsink", "live-output");
	liveProgres = gst_element_factory_make("progressreport", "Live-Progress ");
	if (!bin || !qLive || !sinkLive || !liveProgres) {
		g_printerr(
				"*** One element from initialize Live Sink could not be created. Exiting ***\n");
		return -1;
	}

	/* we set the output live address to the sink element - rtmp://103.226.217.136/live/myStream */
	char serverIp[128];
	sprintf(serverIp, "rtmp://%s/%s/%s",
			(gchar *) getJsonValueFromFile("videoServerIp"),
			(gchar *) getJsonValueFromFile("serverApp"),
			(gchar *) getJsonValueFromFile("streamId"));
	g_object_set(G_OBJECT(sinkLive), "location", serverIp, NULL);
	g_object_set(G_OBJECT(sinkLive), "max-lateness", 100, NULL); //1000000000
	/* we set the output to PAUSE async to filesink */
	g_object_set(G_OBJECT(sinkLive), "async", TRUE, NULL);
	/* we set the live progress report frequency to 2 seconds */
	g_object_set(liveProgres, "update-freq", 2, NULL);
	/* we set the output to PAUSE async to filesink */
	g_object_set(G_OBJECT(qLive), "leaky", 2, NULL);
	/* we set the live progress report frequency to 2 seconds */
	g_object_set(G_OBJECT(qLive), "max-size-buffers", 0, NULL); //50, 200
	g_object_set(G_OBJECT(qLive), "max-size-bytes", 20000000, NULL); //1000000000
	g_object_set(G_OBJECT(qLive), "max-size-time", 10000000, NULL); //3000000000
	/* Obtaining request pads for the tee elements*/
	GstPadTemplate *tee_src_pad_template;
	if (!(tee_src_pad_template = gst_element_class_get_pad_template(
			GST_ELEMENT_GET_CLASS(tee), "src_%u"))) {
		gst_object_unref(pipeline);
		g_critical("*** Unable to get pad template ***");
		return -1;
	}
	/* Manually link the Tee, which has "Request" pads */
	tee_q2_pad = gst_element_request_pad(tee, tee_src_pad_template, NULL, NULL);
	q2_pad = gst_element_get_static_pad(qLive, "sink");
	/* we add all elements into the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), qLive, bin, NULL);
	gst_bin_add_many(GST_BIN(bin), liveProgres, sinkLive, NULL);
	/* Link the tee to the queue 2 */
	if (gst_pad_link(tee_q2_pad, q2_pad) != GST_PAD_LINK_OK) {
		g_critical("*** Tee for q2 could not be linked ***\n");
		gst_object_unref(pipeline);
		return -1;
	}
	gst_object_unref(q2_pad);
	/* we link the elements together */
	gst_element_link_many(liveProgres, sinkLive, NULL);
	/* add ghostpad */
	pad = gst_element_get_static_pad(liveProgres, "sink");
	gst_element_add_pad(bin, gst_ghost_pad_new("sink", pad));
	//g_signal_connect(qLive, "overrun", G_CALLBACK(queue_overrun), NULL);
	gst_object_unref(GST_OBJECT(pad));
	gst_element_link_many(qLive, bin, NULL);
	gst_object_unref(bus);
	return 0;
}

int initialize() {

	/* Initialisation */
	loop = g_main_loop_new(NULL, FALSE);

	/* Create gstreamer elements */
	pipeline = gst_pipeline_new("camera-recoder-h264");
	flvmux = gst_element_factory_make("flvmux", "flv-mux");
	muxProgres = gst_element_factory_make("progressreport", "Mux-Progress  ");
	tee = gst_element_factory_make("tee", "output-tee");

	if (!pipeline || !flvmux || !muxProgres || !tee) {
		g_printerr(
				"*** One element main initialize could not be created. Exiting ***\n");
		return -1;
	}
	/* we set the flvmux property STREAMABLE to TRUE */
	g_object_set(G_OBJECT(flvmux), "streamable", FALSE, NULL);
	/* we set the mux progress report frequency to 2 seconds */
	g_object_set(muxProgres, "update-freq", 2, NULL);
	/* we set the tee to don't omit error when not linked */
	g_object_set(G_OBJECT(tee), "allow-not-linked", TRUE, NULL);
	/* we add a message handler */
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);
	gst_object_unref(bus);
	//g_signal_connect(qAudio, "overrun", G_CALLBACK(queue_overrun), NULL);
	gst_bin_add_many(GST_BIN(pipeline), flvmux, muxProgres, tee, NULL);
	gst_element_link_many(flvmux, muxProgres, tee, NULL);

	if (initCamera() != 0)
		return -1;
	//if (initAudio() != 0)
		//return -1;
	if (initLocal() != 0)
		return -1;
	return 0;
}

int play() {

	/* Set the pipeline to "playing" state*/
	g_print("*** Now playing ***\n");
	int r = gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* Iterate */
	g_print("*** Running ***\n");
	g_main_loop_run(loop);
	return r;
}

int pause() {

	/* Set the pipeline to "playing" state*/
	g_print("*** Running, Now Pausing ***\n");
	int r = gst_element_set_state(pipeline, GST_STATE_PAUSED);

	/* Iterate */
	g_print("*** Paused ***\n");
	return r;
}

void stopLive() {

	gst_element_set_state(bin, GST_STATE_NULL);
	gst_element_set_state(qLive, GST_STATE_NULL);
	gst_element_unlink_many(liveProgres, sinkLive, NULL);
	pad = gst_element_get_static_pad(liveProgres, "sink");
	gst_element_remove_pad(bin, gst_ghost_pad_new("sink", pad));
	gst_object_unref(GST_OBJECT(pad));
	gst_element_unlink_many(qLive, bin, NULL);
	gst_element_remove_pad(tee, tee_q2_pad);
	gst_pad_unlink(tee_q2_pad, q2_pad);
	gst_bin_remove_many(GST_BIN(bin), liveProgres, sinkLive, NULL);
	gst_bin_remove_many(GST_BIN(pipeline), qLive, bin, NULL);
	gst_bin_remove(GST_BIN(pipeline), bin);
	gst_object_unref(GST_OBJECT(bin));
	g_print("*** Live Stopped ***\n");
}

void startLive() {
	gst_element_set_state(pipeline, GST_STATE_PAUSED);
	initLive();
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void *ThreadMain(void *threadArgs) {
	pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */
	play(); /* Starting The loop, Control is hold here until error or stop */
	pthread_exit(NULL);
	return (NULL);
}

void startStreaming() {
	g_thread_new("GST_Thread", ThreadMain, NULL);
}

int stopStreaming() {

	/* Out of the main loop, clean up nicely */
	g_print("*** Mainloop stop: Stopping Stream ***\n");
	g_main_loop_quit(loop);
	int r = gst_element_set_state(pipeline, GST_STATE_NULL);

	g_print("*** return SetState %d ***\n", r);
	g_print("*** Deleting pipeline ***\n");
	gst_object_unref(GST_OBJECT(pipeline));
	g_source_remove(bus_watch_id);
	g_main_loop_unref(loop);
	return r;
}

int setAll() {
	stopStreaming();
	stopLive();
	if (initialize() != 0) {
		printf("Error in initializing GST.\n");
		return -1;
	}

	GST_DEBUG_BIN_TO_DOT_FILE(GST_OBJECT(pipeline), GST_DEBUG_GRAPH_SHOW_ALL,
			"pipeline");

	return 0;
}
