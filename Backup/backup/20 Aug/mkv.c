#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <signal.h>

GMainLoop *loop;
void intHandler(int dummy) {

	g_main_loop_quit (loop);
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

	//signal(SIGINT, intHandler);
	
	GstElement *pipeline, *source, *avimux, *mkvmux, *queueLocal, 
			*queueLive, *sinkLocal, *sinkLive, *tee, *filter,
			*queueNew, *progReport, *bin, *valve;
	GstBus *bus;
	guint bus_watch_id;
	GstCaps *cap;
	GstPad *pad;

	
	/* Initialisation */
	gst_init (&argc, &argv);

	loop = g_main_loop_new (NULL, FALSE);

	/* 
		gst-launch-1.0 v4l2src device=/dev/video1 ! image/jpeg,width=800,height=600,framerate=30/1 ! matroskamux streamable=true ! tee name=t t. ! 			queue ! filesink location=temp.mkv t. ! queue ! bin name=b b. ! valve drop=false ! progressreport update-freq=2 ! 
		rtmpsink location=rtmp://103.226.217.136:1935/live/myStream async=true
	*/
	

	/* Create gstreamer elements */

	pipeline 	= gst_pipeline_new ("camera-recoder-h264");
	source   	= gst_element_factory_make ("v4l2src",	"h264-source");
	queueLocal    	= gst_element_factory_make ("queue",	"queue-local");
	queueLive    	= gst_element_factory_make ("queue",	"queue-live");
	tee     	= gst_element_factory_make ("tee", 	"output-tee");
	sinkLocal     	= gst_element_factory_make ("filesink", "video-file-output");
	sinkLive     	= gst_element_factory_make ("rtmpsink", "video-stream-output");
	valve     	= gst_element_factory_make ("valve", 	"live-valve");
	filter 		= gst_element_factory_make ("capsfilter",	"filter");
	mkvmux  	= gst_element_factory_make ("matroskamux",	"mkv-mux");//matroskamux
	progReport     	= gst_element_factory_make ("progressreport", 	"progress-report");
	cap 	 	= gst_caps_from_string ("video/x-h264,width=800,height=600,framerate=30/1");
	bin		= gst_bin_new ("liveBin");
	
	if ( !pipeline || !source || !mkvmux || !queueLocal || !queueLive || !tee || !sinkLocal || !sinkLive  || !cap || !progReport || !bin || !valve || !filter ) {
		g_printerr ("*** One element could not be created. Exiting ***\n");
		return -1;
	}


	/* Set up the pipeline */

	/* we set the input camera device to the source element */
	g_object_set (G_OBJECT (source), "device", "/dev/video2", NULL);

	/* we set the mkvmux property STREAMABLE to TRUE */
	g_object_set (G_OBJECT (mkvmux),"streamable", 1, NULL);	

	/* we set the capabilities of camera using cap filter */
	g_object_set(filter,"caps",cap,NULL);	

	/* we set the output local filename to the sink element */
	g_object_set (G_OBJECT (sinkLocal), "location", "temp.mkv", NULL);
	
	/* we set the output live address to the sink element - rtmp://103.226.217.136/live/myStream */
	g_object_set (G_OBJECT (sinkLive), "location", "rtmp://103.226.217.136/live/myStream", NULL);//"temp2.mkv"
	
	/* we set the output to PAUSE async to filesink */
	g_object_set (G_OBJECT (sinkLive), "async", FALSE, NULL);
	
	/* we set the progress report frequency to 2 seconds */
	g_object_set (progReport, "update-freq", 2, NULL);

	/* we set the valve to no-drop */
	g_object_set (valve, "drop", FALSE, NULL);

	/* we add a message handler */
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);

   	gst_object_unref (bus);


	/* we add all elements into the pipeline */
	/* camera-source | jpeg-avimux | queueLocal erter | windows-output */
	gst_bin_add_many (GST_BIN (pipeline), source, mkvmux, queueLocal, tee, queueLive, sinkLocal, NULL); //sinkLive, progReport, 
	
	gst_element_link_filtered((GstElement*) source, mkvmux, cap);

	gst_element_link_many(mkvmux, tee, NULL);


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
	g_print ("*** Obtained request pad %s for q1 branch ***\n", gst_pad_get_name (tee_q1_pad));
	q1_pad = gst_element_get_static_pad (queueLocal, "sink");

	tee_q2_pad = gst_element_request_pad (tee, tee_src_pad_template, NULL, NULL);
	g_print ("*** Obtained request pad %s for q2 branch ***\n", gst_pad_get_name (tee_q2_pad));
	q2_pad = gst_element_get_static_pad (queueLive, "sink");
	
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
	/* camera-source -> jpeg-avimux -> queueLocal -> windows-output */

	/* Link the camera source and avimux filter using capabilities specified */
	gst_element_link_many (queueLocal, sinkLocal, NULL);

	gst_bin_add_many (GST_BIN (bin), valve, progReport, sinkLive, NULL);
	gst_element_link_many (valve, progReport, sinkLive, NULL);

	/* add ghostpad */
	pad = gst_element_get_static_pad (valve, "sink");
	gst_element_add_pad (bin, gst_ghost_pad_new ("sink", pad));
	gst_object_unref (GST_OBJECT (pad));
	
	gst_element_link_many (queueLive, bin, NULL);
	
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
