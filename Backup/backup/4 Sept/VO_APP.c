#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

GMainLoop *loop;
GstElement *bin;
gboolean firstInt = TRUE;

void intHandler(int dummy) {
	g_print ("*** SIGINT Caught ***\n");
	if (firstInt) {
		g_main_loop_quit (loop);
		firstInt = FALSE;
		return;
	}
	exit(0);
}

static void queue_overrun(GstElement *queue, gpointer data) {

	g_print ("*** Overrun Caught ***\n");
	guint mBuf, mByt, mTim, cBuf, cByt, cTim;

	g_object_get (G_OBJECT (queue), "max-size-buffers", &mBuf, NULL);
	g_object_get (G_OBJECT (queue), "max-size-bytes", &mByt, NULL);
	g_object_get (G_OBJECT (queue), "max-size-time", &mTim, NULL);
	g_object_get (G_OBJECT (queue), "current-level-buffers", &cBuf, NULL);
	g_object_get (G_OBJECT (queue), "current-level-bytes", &cByt, NULL);
	g_object_get (G_OBJECT (queue), "current-level-time", &cTim, NULL);

	//g_print ("Status: %u, %u, %u, %u, %u, %u\n",mBuf, cBuf, mByt, cByt, mTim, cTim);
}

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) {
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE (msg)) {

		case GST_MESSAGE_EOS:
			g_print ("*** End of stream ***\n");
			g_main_loop_quit (loop);
			break;

		case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;

			gst_message_parse_error (msg, &error, &debug);
			g_free (debug);

			g_printerr ("*** ERROR: %s ***\n", error->message);
			g_error_free (error);

			g_main_loop_quit (loop);
			break;
		}

		case GST_MESSAGE_INFO: {
			gchar  *debug;
			GError *error;

			gst_message_parse_info (msg, &error, &debug);
			g_free (debug);

			g_print ("*** INFO: %s ***\n", error->message);
			g_error_free (error);

			g_main_loop_quit (loop);
			break;
		}

		default:
			break;
	}

	return TRUE;
}

int main (int argc, char *argv[]) {

	signal(SIGINT, intHandler);
	
	GstElement	*pipeline, *source, *flvmux, *qCam, *qLocal, *muxProgres, 
			*sinkLocal, *sinkLive, *tee, *filter, *qLive, 
			*parse, *lclProgres, *liveProgres;
	GstBus *bus;
	guint bus_watch_id;
	GstCaps *cap;
	GstPad *pad;

	
	/* Initialisation */
	gst_init (&argc, &argv);

	loop = g_main_loop_new (NULL, FALSE);

	/*
		gst-launch-1.0 -v v4l2src device=/dev/video2 ! video/x-h264, framerate=10/1, width=640, height=480 ! queue2 ! h264parse config-interval=96 ! progressreport name=mux ! flvmux ! tee name=t ! queue2 ! progressreport name=local ! filesink location=mux.flv t. ! queue2 ! progressreport name=live ! rtmpsink location=rtmp://103.226.217.136:1935/live/myStream

		queue2 max-size-buffers=500 ring-buffer-max-size=5000000

	*/

	/* Create gstreamer elements */

	pipeline 	= gst_pipeline_new ("camera-recoder-h264");
	source   	= gst_element_factory_make ("v4l2src",		"h264-source");
	qCam    	= gst_element_factory_make ("queue",		"queue-camera");
	qLocal    	= gst_element_factory_make ("queue",		"queue-local");
	qLive    	= gst_element_factory_make ("queue",		"queue-live");
	parse 		= gst_element_factory_make ("h264parse",	"h264-parse");
	flvmux  	= gst_element_factory_make ("flvmux",		"flv-mux");
	tee     	= gst_element_factory_make ("tee", 		"output-tee");
	sinkLocal    	= gst_element_factory_make ("filesink", 	"local-output");
	sinkLive     	= gst_element_factory_make ("rtmpsink", 	"live-output");
	muxProgres     	= gst_element_factory_make ("progressreport", 	"Mux-Progress  ");
	lclProgres     	= gst_element_factory_make ("progressreport", 	"Local-Progress");
	liveProgres	= gst_element_factory_make ("progressreport", 	"Live-Progress ");
	filter 		= gst_element_factory_make ("capsfilter",	"filter");
	cap 	 	= gst_caps_from_string ("video/x-h264,framerate=10/1,width=640,height=480");
	//valve     	= gst_element_factory_make ("valve", 		"live-valve");
	bin		= gst_bin_new ("liveBin");
	
	if ( 	    !pipeline
		 || !source
		 || !qCam
		 || !qLocal
		 || !qLive
		 || !flvmux
		 || !tee
		 || !sinkLocal
		 || !sinkLive
		 || !cap
		 || !lclProgres
		 || !liveProgres
		 || !filter
		 || !bin
		 ) {
		g_printerr ("*** One element could not be created. Exiting ***\n");
		return -1;
	}


	/* Set up the pipeline */

	/* we set the input camera device to the source element */
	g_object_set (G_OBJECT (source), "device", "/dev/video1", NULL);

	/* we set the flvmux property STREAMABLE to TRUE */
	g_object_set (G_OBJECT (flvmux),"streamable", FALSE, NULL);	

	/* we set the capabilities of camera using cap filter */
	g_object_set(filter,"caps",cap,NULL);	

	/* we set the output local filename to the sink element */
	g_object_set (G_OBJECT (sinkLocal), "location", "/root/videos/mux.flv", NULL);
	
	/* we set the output live address to the sink element - rtmp://103.226.217.136/live/myStream */
	g_object_set (G_OBJECT (sinkLive), "location", "rtmp://103.226.217.136/live/myStream", NULL);
	
	/* we set the output to PAUSE async to filesink */
	g_object_set (G_OBJECT (sinkLive), "async", TRUE, NULL);
	
	/* we set the local progress report frequency to 2 seconds */
	g_object_set (lclProgres, "update-freq", 2, NULL);
	g_object_set (liveProgres, "update-freq", 2, NULL);
	g_object_set (muxProgres, "update-freq", 2, NULL);

	/* we set the output to PAUSE async to filesink */
	g_object_set (G_OBJECT (qLive), "leaky", 2, NULL);
	
	/* we set the live progress report frequency to 2 seconds */
	g_object_set (qLive, "max-size-time", 3000000000, NULL);
	g_object_set (qLive, "max-size-buffers", 50, NULL);
	g_object_set (qLive, "max-size-bytes", 1000000000000, NULL);

	/* we set the valve to no-drop */
	//g_object_set (valve, "drop", FALSE, NULL);

	/* we add a message handler */
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);

   	gst_object_unref (bus);

	g_signal_connect(qLive, "overrun", G_CALLBACK (queue_overrun), NULL);


	/* we add all elements into the pipeline */
	gst_bin_add_many (GST_BIN (pipeline), source, qCam, parse, flvmux, muxProgres, tee, qLocal, lclProgres, sinkLocal, qLive, bin, NULL); // liveProgres, sinkLive,
	

	/* we link the elements together */
	gst_element_link_filtered((GstElement*) source, qCam, cap);
	gst_element_link_many(qCam, parse, flvmux, muxProgres, tee, NULL);


	/* Manually link the Tee, which has "Request" pads */
	GstPadTemplate *tee_src_pad_template;
	GstPad *tee_q1_pad, *tee_q2_pad;
	GstPad *q1_pad, *q2_pad;

	if ( !(tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (tee), "src_%u"))) {
		gst_object_unref (pipeline);
		g_critical ("*** Unable to get pad template ***");
		return 0;  
	}

	/* Obtaining request pads for the tee elements*/
	tee_q1_pad = gst_element_request_pad (tee, tee_src_pad_template, NULL, NULL);
	q1_pad = gst_element_get_static_pad (qLocal, "sink");

	tee_q2_pad = gst_element_request_pad (tee, tee_src_pad_template, NULL, NULL);
	q2_pad = gst_element_get_static_pad (qLive, "sink");
	
	/* Link the tee to the queue 1 */
	if (gst_pad_link(tee_q1_pad, q1_pad) != GST_PAD_LINK_OK ){
		g_critical ("*** Tee for q1 could not be linked ***\n");
		gst_object_unref (pipeline);
		return 0;
	}
	
	/* Link the tee to the queue 2 */
	if (gst_pad_link(tee_q2_pad, q2_pad) != GST_PAD_LINK_OK) {
		g_critical ("*** Tee for q2 could not be linked ***\n");
		gst_object_unref (pipeline);
		return 0;
	}
 
	gst_object_unref (q1_pad);
	gst_object_unref (q2_pad);


	/* we link the elements together */
	/* camera-source -> jpeg-avimux -> qLocal -> windows-output */

	gst_element_link_many (qLocal, lclProgres, sinkLocal, NULL);

	gst_bin_add_many (GST_BIN (bin), liveProgres, sinkLive,NULL);
	gst_element_link_many (liveProgres, sinkLive, NULL);

	/* add ghostpad */
	pad = gst_element_get_static_pad (liveProgres, "sink");
	gst_element_add_pad (bin, gst_ghost_pad_new ("sink", pad));
	gst_object_unref (GST_OBJECT (pad));
	
	gst_element_link_many (qLive, bin, NULL);

	//gst_element_link_many (qLive, liveProgres, sinkLive, NULL);

	/* Set the pipeline to "playing" state*/
	g_print ("*** Now playing ***\n");
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	/* Iterate */
	g_print ("*** Running... ***\n");
	g_main_loop_run (loop);


	/* Out of the main loop, clean up nicely */
	g_print ("*** Mainloop stop: stopping Stream ***\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);

	g_print ("*** Deleting pipeline ***\n");
	gst_object_unref (GST_OBJECT (pipeline));
	g_source_remove (bus_watch_id);
	g_main_loop_unref (loop);

	return 0;
}