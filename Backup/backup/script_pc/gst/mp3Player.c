#include <gst/gst.h>
#include <glib.h>

static gboolean cb_print_position (GstElement *pipeline) {
	gint64 pos, len;
	if (gst_element_query_position (pipeline, GST_FORMAT_TIME, &pos) && gst_element_query_duration (pipeline, GST_FORMAT_TIME, &len)) {
		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
		GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
	}
	/* call me again */
	return TRUE;
}

static gboolean seek_to_time (GstElement *pipeline) {
	gint64 time_nanoseconds = 90*10^9;
	if (!gst_element_seek (pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
		GST_SEEK_TYPE_SET, time_nanoseconds,
		GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
		g_print ("Seek failed!\n");
	}
	return FALSE;
}

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) {
	GMainLoop *loop = (GMainLoop *) data;
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS:
			g_print ("End of stream\n");
			g_main_loop_quit (loop);
			break;
		case GST_MESSAGE_ERROR: {
			gchar *debug;
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
	GstElement *pipeline, *source, *decoder, *conv, *sink;
	GstBus *bus;
	guint bus_watch_id;
	
	/* Initialisation */
	gst_init (&argc, &argv);
	loop = g_main_loop_new (NULL, FALSE);

	/* Check input arguments */
	if (argc < 2) {
		g_printerr ("Usage: %s <mp3 filename>\n", argv[0]);
		return -1;
	}
	
	/* Create gstreamer elements */
	pipeline = gst_pipeline_new ("audio-player");
	source = gst_element_factory_make ("filesrc", "file-source");
	decoder = gst_element_factory_make ("mad", "mp3-decoder");
	conv = gst_element_factory_make ("audioconvert", "converter");
	sink = gst_element_factory_make ("autoaudiosink", "audio-output");
	if (!pipeline || !source || !decoder || !conv || !sink) {
		g_printerr ("One element could not be created. Exiting.\n");
		return -1;
	}

	/* Set up the pipeline */
	/* we set the input filename to the source element */
	g_object_set (G_OBJECT (source), "location", argv[1], NULL);

	/* we add a message handler */
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	/* we add all elements into the pipeline */
	/* file-source | mp3-decoder | converter | alsa-output */
	gst_bin_add_many (GST_BIN (pipeline), source, decoder, conv, sink, NULL);

	/* we link the elements together */
	/* file-source ~> mp3-decoder -> converter -> alsa-output */
	gst_element_link_many (source, decoder, conv, sink, NULL);

	/* Set the pipeline to "playing" state*/
	g_print ("Now playing: %s\n", argv[1]);
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	/* Setting timer to display position */
	g_timeout_add (200 /*200 ms*/, (GSourceFunc) cb_print_position, pipeline);

	/* Setting timer to seek position */
	// g_timeout_add (2000 /*2000 ms*/, (GSourceFunc) seek_to_time, pipeline);

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
