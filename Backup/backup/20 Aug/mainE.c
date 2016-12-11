#include <gst/gst.h>
#include <glib.h>


static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) {
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE (msg)) {

		case GST_MESSAGE_EOS:
			g_print ("End of stream\n");
			g_main_loop_quit (loop);
			break;

		case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;

			gst_message_parse_error (msg, &error, &debug);
			g_free (debug);

			g_printerr ("Error: %s\n", error->message);
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
	GMainLoop *loop;

	GstElement *pipeline, *source, *muxer, *queueLocal, *sinkLocal, *filter;
	GstBus *bus;
	guint bus_watch_id;
	GstCaps *cap;

	/* Initialisation */
	gst_init (&argc, &argv);

	loop = g_main_loop_new (NULL, FALSE);

	// gst-launch-1.0 v4l2src device=/dev/video1 ! image/jpeg,width=1280,height=720,framerate=30/1 ! avimux ! filesink location=temp.avi
	/* Create gstreamer elements */

	pipeline 	= gst_pipeline_new ("camera-recoder");
	source   	= gst_element_factory_make ("v4l2src",	"camera-source");
	muxer  		= gst_element_factory_make ("avimux",	"avi-muxer");
	queueLocal    	= gst_element_factory_make ("queue",	"queue-local");
	sinkLocal     	= gst_element_factory_make ("filesink", "video-file-output");
	filter 		= gst_element_factory_make ("capsfilter",	"filter");
	cap 	 	= gst_caps_from_string("image/jpeg,width=800,height=600,framerate=30/1");
	
	if ( !pipeline || !source || !muxer || !queueLocal || !sinkLocal  || !cap) {
		g_printerr ("One element could not be created. Exiting.\n");
		return -1;
	}

	/* Set up the pipeline */

	/* we set the input camera device to the source element */
	g_object_set (G_OBJECT (source), "device", "/dev/video1", NULL);
	
	/* we set the capabilities of camera using cap filter */
	g_object_set(filter,"caps",cap,NULL);	

	/* we set the output local filename to the sink element */
	g_object_set (G_OBJECT (sinkLocal), "location", "temp.avi", NULL);

	/* we add a message handler */
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	/* we add all elements into the pipeline */
	/* camera-source | jpeg-muxer | queueLocalerter | windows-output */
	gst_bin_add_many (GST_BIN (pipeline), source, muxer, queueLocal, sinkLocal, NULL);

	/* we link the elements together */
	/* camera-source -> jpeg-muxer -> queueLocal -> windows-output */
	
	/* Link the camera source and muxer filter using capabilities specified */
	gst_element_link_filtered((GstElement*) source, muxer, cap);
	gst_element_link_many (muxer, queueLocal, sinkLocal, NULL);
	//gst_element_link_many (muxer, queueLive, sinkLive, NULL);



	/* Set the pipeline to "playing" state*/
	g_print ("Now playing: %s\n", argv[1]);
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	/* Iterate */
	g_print ("Running...\n");
	g_main_loop_run (loop);



	/* Out of the main loop, clean up nicely */
	g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);

	g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));
	g_source_remove (bus_watch_id);
	g_main_loop_unref (loop);

	return 0;
}

