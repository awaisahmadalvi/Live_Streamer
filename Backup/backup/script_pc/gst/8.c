#include <gst/gst.h>
int main (int argc, char *argv[]) {
	GstElement *player;
	
	/* init */
	gst_init (&argc, &argv);

	/* create player */
	player = gst_element_factory_make ("oggvorbisplayer", "player");

	/* set the source audio file */
	g_object_set (player, "location", "/root/script/gst/hello.ogg", NULL);

	/* start playback */
	gst_element_set_state (GST_ELEMENT (player), GST_STATE_PLAYING);
}
