#! /bin/sh
# Starting Stream on usb, wlan or eth

if [ "$IFACE" = lo ]; then
        echo "Streaming exit, lo" >> /dev/kmsg
        exit 0
fi
if [ "$IFACE" = --all ]; then
        echo "Streaming exit, --all" >> /dev/kmsg
        exit 0
fi
# Only run from ifup.
if [ "$MODE" != start ]; then
        echo "Streaming exit, !start" >> /dev/kmsg
        exit 0
fi
#if [ "$ADDRFAM" != inet ] && [ "$ADDRFAM" != inet6 ]; then
#        echo "Streaming exit, inet" >> /dev/kmsg
#       exit 0
#fi

while [ ! -r /dev/video0 ]; do
        echo "device not ready, /dev/video0" >> /dev/kmsg
        sleep 1
done
{
echo "device ready, /dev/video0" >> /dev/kmsg
echo "Streaming wait for 5 sec" >> /dev/kmsg
sleep 15
echo "Streaming Started via $IFACE @ $(date)" >> /var/log/stream
echo "Streaming Started via $IFACE @ $(date)" >> /dev/kmsg
ffmpeg -f v4l2 -video_size 640x480 -framerate 15 -i /dev/video0 -f flv -threads$
}&


