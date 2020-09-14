#! /bin/bash
clear
gcc $1 `pkg-config --cflags --libs gstreamer-1.0 glib-2.0 glib-2.0 gobject-2.0 x11`

res=$?
if [ $res == 0 ]; then
	./a.out $2 $3
	exit 0
fi
echo "Error!!!!!!"
