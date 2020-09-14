#! /bin/bash


cd /root/script/logs/local/

(ffmpeg -framerate 30 -video_size hd720 -i /dev/video1 -c:v libx264 -preset ultrafast -qp 15 /root/script/output.mkv -y -report)&
until [ -e /root/script/output.mkv ]; do 
	sleep 2
	echo 'waiting for mkv'  | tee -a /root/script/script.log >> /dev/kmsg
done
sleep 10
echo 'live Stream Starting'  | tee -a /root/script/script.log >> /dev/kmsg

(sh /root/script/live.sh)
