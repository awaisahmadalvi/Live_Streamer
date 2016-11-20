//export GST_DEBUG="alsa*:6"

#include <gst/gst.h>
#include <json/json.h>
#include <glib.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <../TCPClient/client.h>

GMainLoop *loop;
GstElement *pipeline, *bin, *vSource, *qCam, *filter,
//			*aSource, *aRateConv, *qAudio, *aacmux,
		*flvmux, *qLive, *tee, *qLocal, *parse, *sinkLocal, *sinkLive,
		*lclProgres, *liveProgres, *muxProgres;

GstPad *tee_q1_pad, *tee_q2_pad, *q1_pad, *q2_pad;

GstBus *bus;
guint bus_watch_id;
GstCaps *vCap, *aCap;
GstPad *pad;

gboolean firstInt = TRUE;
char timeStr[20];

void stop();

void intHandler(int dummy) {

	g_print("*** SIGINT Caught ***\n");
	if (firstInt) {
		g_main_loop_quit(loop);
		firstInt = FALSE;
		return;
	}
	stop();
	exit(0);
}

void getCurTime() {
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeStr, 16, "%H%M%S_%y%m%d", timeinfo);
}

static void queue_overrun(GstElement *queue, gpointer data) {

	g_print("*** Overrun Caught ***\n");
	/*
	 guint mBuf, mByt, mTim, cBuf, cByt, cTim;

	 g_object_get (G_OBJECT (queue), "max-size-time", &mBuf, NULL);
	 g_object_get (G_OBJECT (queue), "max-size-bytes", &mByt, NULL);
	 g_object_get (G_OBJECT (queue), "max-size-time", &mTim, NULL);
	 g_object_get (G_OBJECT (queue), "current-level-time", &cBuf, NULL);
	 g_object_get (G_OBJECT (queue), "current-level-bytes", &cByt, NULL);
	 g_object_get (G_OBJECT (queue), "current-level-time", &cTim, NULL);

	 g_print ("Status: %u, %u, %u, %u, %u, %u\n",mBuf, cBuf, mByt/1000/1000,
	 cByt/1000/1000, mTim/100000000, cTim/100000000);
	 */
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE(msg)) {

	case GST_MESSAGE_EOS:
		g_print("*** End of stream ***\n");
		g_main_loop_quit(loop);
		break;

	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *error;

		gst_message_parse_error(msg, &error, &debug);
		/*
		 gst_object_default_error (GST_MESSAGE_SRC (msg), error, debug);
		 gst_message_unref (msg);
		 */
		g_printerr("*** ERROR: %s ***\n", error->message);
		g_error_free(error);
		g_free(debug);
		g_main_loop_quit(loop);
		break;
	}

	case GST_MESSAGE_INFO: {
		gchar *debug;
		GError *error;

		gst_message_parse_info(msg, &error, &debug);
		g_free(debug);

		g_print("*** INFO: %s ***\n", error->message);
		g_error_free(error);

		g_main_loop_quit(loop);
		break;
	}

	default:
		break;
	}

	return TRUE;
}

int initialize() {
	int argc;
	char *argv[0];
	/* Initialisation */
	gst_init(&argc, &argv);

	loop = g_main_loop_new(NULL, FALSE);

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

	 */

	/* Create gstreamer elements */

	pipeline = gst_pipeline_new("camera-recoder-h264");
	//aSource = gst_element_factory_make("alsasrc", "alsa-aSource");
	//aRateConv = gst_element_factory_make("audiorate", "audio-converter");
	vSource = gst_element_factory_make("v4l2src", "h264-vSource");
	//qAudio = gst_element_factory_make("queue", "queue-audio");
	qCam = gst_element_factory_make("queue", "queue-camera");
	qLocal = gst_element_factory_make("queue", "queue-local");
	qLive = gst_element_factory_make("queue", "queue-live");
	parse = gst_element_factory_make("h264parse", "h264-parse");
	flvmux = gst_element_factory_make("flvmux", "flv-mux");
	//aacmux = gst_element_factory_make("speexenc", "aac-mux");
	tee = gst_element_factory_make("tee", "output-tee");
	sinkLocal = gst_element_factory_make("filesink", "local-output");
	sinkLive = gst_element_factory_make("rtmpsink", "live-output");
	muxProgres = gst_element_factory_make("progressreport", "Mux-Progress  ");
	lclProgres = gst_element_factory_make("progressreport", "Local-Progress");
	liveProgres = gst_element_factory_make("progressreport", "Live-Progress ");
	vCap = gst_caps_from_string((gchar *) getJsonValueFromFile("v4l2caps"));
	//aCap = gst_caps_from_string((gchar *) getJsonValueFromFile("audiocaps"));
	//filter 		= gst_element_factory_make ("capsfilter",	"filter");
	//valve     	= gst_element_factory_make ("valve", "live-valve");
	bin = gst_bin_new("liveBin");

	if (!pipeline || !vSource || !qCam || !qLocal || !qLive || !flvmux || !tee
			|| !sinkLocal || !sinkLive || !vCap || !lclProgres || !liveProgres
//		    || !filter || !valve || !aSource || !aRateConv || !qAudio || !aCap || !aacmux
			|| !bin) {
		g_printerr("*** One element could not be created. Exiting ***\n");
		return -1;
	}
	return 0;
}

void setProp() {

	/* Set up the pipeline */

	/* we set the input Camera device to the vSource element */
	g_object_set(G_OBJECT(vSource), "device",
			(gchar *) getJsonValueFromFile("v4l2device"), NULL);

	/* we set the input Pulse Audio device to the aSource element */
	//g_object_set(G_OBJECT(aSource), "device",(gchar *) getJsonValueFromFile("audiodevice"), NULL);
	/* we set the flvmux property STREAMABLE to TRUE */
	g_object_set(G_OBJECT(flvmux), "streamable", FALSE, NULL);

	/* we set the Capabilities of camera using vCap filter */
	//	g_object_set(filter,"caps",vCap,NULL);
	/* we set the output local filename to the sink element */
	getCurTime();
	char loc[128];
	sprintf(timeStr, "%s.flv", timeStr);
	sprintf(loc, "%s%s", (gchar *) getJsonValueFromFile("location"), timeStr);
	g_object_set(G_OBJECT(sinkLocal), "location", loc, NULL);
	g_object_set(G_OBJECT(sinkLocal), "enable-last-sample", FALSE, NULL);

	/* we set the output live address to the sink element - rtmp://103.226.217.136/live/myStream */
	char serverIp[128];
	sprintf(serverIp, "rtmp://%s/%s/%s",
			(gchar *) getJsonValueFromFile("serverIp"),
			(gchar *) getJsonValueFromFile("serverApp"),
			(gchar *) getJsonValueFromFile("streamId"));
	g_object_set(G_OBJECT(sinkLive), "location", serverIp, NULL);
	g_object_set(G_OBJECT(sinkLive), "max-lateness", 1000000000, NULL);

	/* we set the output to PAUSE async to filesink */
	g_object_set(G_OBJECT(sinkLive), "async", TRUE, NULL);

	/* we set the local progress report frequency to 2 seconds */
	g_object_set(lclProgres, "update-freq", 2, NULL);
	g_object_set(liveProgres, "update-freq", 2, NULL);
	g_object_set(muxProgres, "update-freq", 2, NULL);

	/* we set the output to PAUSE async to filesink */
	g_object_set(G_OBJECT(qLive), "leaky", 2, NULL);

	/* we set the live progress report frequency to 2 seconds */
	g_object_set(qLive, "max-size-time", 0, NULL); //50, 200
	g_object_set(qLive, "max-size-bytes", 0, NULL); //1000000000
	g_object_set(qLive, "max-size-time", 3000000000, NULL); //3000000000

	/* we set the valve to no-drop */
	//g_object_set(valve, "drop", FALSE, NULL);
	/* we add a message handler */
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);

	gst_object_unref(bus);

	g_signal_connect(qLive, "overrun", G_CALLBACK(queue_overrun), NULL);

}

int setTee() {

	/* Manually link the Tee, which has "Request" pads */

	GstPadTemplate *tee_src_pad_template;

	if (!(tee_src_pad_template = gst_element_class_get_pad_template(
			GST_ELEMENT_GET_CLASS(tee), "src_%u"))) {
		gst_object_unref(pipeline);
		g_critical("*** Unable to get pad template ***");
		return -1;
	}

	/* Obtaining request pads for the tee elements*/
	tee_q1_pad = gst_element_request_pad(tee, tee_src_pad_template, NULL, NULL);
	q1_pad = gst_element_get_static_pad(qLocal, "sink");

	tee_q2_pad = gst_element_request_pad(tee, tee_src_pad_template, NULL, NULL);
	q2_pad = gst_element_get_static_pad(qLive, "sink");
	return 0;
}

int linkAll() {

	/* we add all elements into the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), vSource, qCam,
			//aSource, aRateConv, qAudio, aacmux,
			parse, flvmux, muxProgres, tee, qLocal, lclProgres, sinkLocal,
			qLive, bin, NULL); // liveProgres, sinkLive,

	/* we link the elements together */
	gst_element_link_filtered((GstElement*) vSource, qCam, vCap);
	//gst_element_link_filtered((GstElement*) aSource, aRateConv, aCap);
	//gst_element_link_many(aRateConv, qAudio, aacmux, flvmux, NULL);
	gst_element_link_many(qCam, parse, flvmux, muxProgres, tee, NULL);

	/* Link the tee to the queue 1 */
	if (gst_pad_link(tee_q1_pad, q1_pad) != GST_PAD_LINK_OK) {
		g_critical("*** Tee for q1 could not be linked ***\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* Link the tee to the queue 2 */
	if (gst_pad_link(tee_q2_pad, q2_pad) != GST_PAD_LINK_OK) {
		g_critical("*** Tee for q2 could not be linked ***\n");
		gst_object_unref(pipeline);
		return -1;
	}

	gst_object_unref(q1_pad);
	gst_object_unref(q2_pad);

	/* we link the elements together */
	/* camera-vSource -> jpeg-avimux -> qLocal -> windows-output */

	gst_element_link_many(qLocal, lclProgres, sinkLocal, NULL);

	gst_bin_add_many(GST_BIN(bin), liveProgres, sinkLive, NULL);
	gst_element_link_many(liveProgres, sinkLive, NULL);

	/* add ghostpad */
	pad = gst_element_get_static_pad(liveProgres, "sink");
	gst_element_add_pad(bin, gst_ghost_pad_new("sink", pad));
	gst_object_unref(GST_OBJECT(pad));

	gst_element_link_many(qLive, bin, NULL);

	//gst_element_link_many (qLive, liveProgres, sinkLive, NULL);

	return 0;
}

void play() {

	/* Set the pipeline to "playing" state*/
	g_print("*** Now playing ***\n");
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* Iterate */
	g_print("*** Running... ***\n");
	g_main_loop_run(loop);
}

void stop() {

	/* Out of the main loop, clean up nicely */
	g_print("*** Mainloop stop: stopping Stream ***\n");
	gst_element_set_state(pipeline, GST_STATE_NULL);

	g_print("*** Deleting pipeline ***\n");
	gst_object_unref(GST_OBJECT(pipeline));
	g_source_remove(bus_watch_id);
	g_main_loop_unref(loop);
}
int gstreamer_main() {
	signal(SIGINT, intHandler);

	guint ret = initialize();
	if (ret < 0)
		return -1;

	setProp();
	ret = setTee();
	if (ret < 0)
		return -1;

	linkAll();
	/* Starting The loop, Control is hold here until error or stop */
	play();
	stop();
	return 0;
}
