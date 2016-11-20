#!/bin/bash










SOURCE=$HOME/GST/gstreamer_sources
BUILD=$HOME/GST/gstreamer_build
if [ ! -d "$SOURCE" ]; then
	mkdir $SOURCE
fi
if [ ! -d "$BUILD" ]; then
	mkdir $BUILD
fi

cd $SOURCE
wget http://gstreamer.freedesktop.org/src/gstreamer/gstreamer-1.8.2.tar.xz
tar -xJf gstreamer-1.8.2.tar.xz
wget http://gstreamer.freedesktop.org/src/orc/orc-0.4.18.tar.gz
tar -xzf orc-0.4.18.tar.gz
wget http://gstreamer.freedesktop.org/src/gst-plugins-base/gst-plugins-base-1.8.2.tar.xz
tar -xJf gst-plugins-base-1.8.2.tar.xz
wget http://gstreamer.freedesktop.org/src/gst-plugins-good/gst-plugins-good-1.8.2.tar.xz
tar -xJf gst-plugins-good-1.8.2.tar.xz
wget http://gstreamer.freedesktop.org/src/gst-plugins-ugly/gst-plugins-ugly-1.8.2.tar.xz
tar -xJf gst-plugins-ugly-1.8.2.tar.xz
wget http://gstreamer.freedesktop.org/src/gst-plugins-bad/gst-plugins-bad-1.8.2.tar.xz
tar -xJf gst-plugins-bad-1.8.2.tar.xz
wget http://gstreamer.freedesktop.org/src/gst-libav/gst-libav-1.8.2.tar.xz
tar -xJf gst-libav-1.8.2.tar.xz
<<comment
cd gstreamer-1.8.2
./configure --prefix=$BUILD
make -j2
make install

cd ../orc-0.4.18
./configure --prefix=$BUILD 
make -j2
make install

cd ../gst-plugins-base-1.8.2
export PKG_CONFIG_PATH=$BUILD/lib/pkgconfig:$PKG_CONFIG_PATH; ./configure --prefix=$BUILD  --enable-orc --with-x 
make -j2
make install 

cd ../gst-plugins-good-1.8.2
export PKG_CONFIG_PATH=$BUILD/lib/pkgconfig:$PKG_CONFIG_PATH; ./configure  --prefix=$BUILD  --enable-orc --with-libv4l2 --with-x
make -j2
make install 

cd ../gst-plugins-ugly-1.8.2
export PKG_CONFIG_PATH=$BUILD/lib/pkgconfig:$PKG_CONFIG_PATH; ./configure  --prefix=$BUILD  --enable-orc 
make -j2
make install

cd ../gst-plugins-bad-1.8.2
export PKG_CONFIG_PATH=$BUILD/lib/pkgconfig:$PKG_CONFIG_PATH; ./configure  --prefix=$BUILD  --enable-orc
make -j2
make install

cd ../gst-libav-1.8.2
export PKG_CONFIG_PATH=$BUILD/lib/pkgconfig:$PKG_CONFIG_PATH; ./configure  --prefix=$BUILD  --enable-orc
make -j2
make install

comment
echo
echo "GStreamer 1.8.2 installed sucessfully!"
echo

# to completly remove gstreamer, simpely delete the two directories:
# rm -rf $SOURCE
# rm -rf $BUILD

# to use this version of gstreamer, add it to your PATH:
# echo -e "\n\n#Path for a local version of gstreamer\nPATH=\"$BUILD/bin:\$PATH\"" >> $HOME/.profile
