#! /bin/bash

gcc $1 `pkg-config --cflags --libs gstreamer-1.0 glib-2.0 glib-2.0 gobject-2.0 x11`
until [ -e a.out ]; do
	sleep 1
done
echo ""

./a.out $2 $3
echo ""

