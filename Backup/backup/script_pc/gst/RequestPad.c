static void some_function (GstElement *tee) {

	GstPad * pad;
	gchar *name;
	pad = gst_element_get_request_pad (tee, "src%d");
	name = gst_pad_get_name (pad);
	g_print ("A new pad %s was created\n", name);
	g_free (name);

	/* here, you would link the pad */
	//	[..]

	/* and, after doing that, free our reference */
	gst_object_unref (GST_OBJECT (pad));
}
