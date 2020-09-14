#! /bin/bash

#sh /root/script/ffstop.sh 
sh /root/script/movelog.sh
sleep 1
cd /root/script/
(ffmpeg -re -r 10 -i /root/script/output.mkv -filter:v scale=480:-1 -b:v 1500k -threads 20 -f flv rtmp://103.226.217.136/live/myStream -report )&
FOO_PID=$!
echo PID=$FOO_PID | tee -a /root/script/script.log >> /dev/kmsg
exit 0
