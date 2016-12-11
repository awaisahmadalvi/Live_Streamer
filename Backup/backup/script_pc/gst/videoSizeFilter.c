static gboolean link_elements_with_filter (GstElement *element1, GstElement *element2)
{
	gboolean link_ok;
	GstCaps *caps;
	caps = gst_caps_new_simple ("video/x-raw",
		"format", G_TYPE_STRING, "I420",
		"width", G_TYPE_INT, 384,
		"height", G_TYPE_INT, 288,
		"framerate", GST_TYPE_FRACTION, 25, 1,
		NULL);
	link_ok = gst_element_link_filtered (element1, element2, caps);
	gst_caps_unref (caps);
	if (!link_ok) {
		g_warning ("Failed to link element1 and element2!");
	}
	return link_ok;
}
