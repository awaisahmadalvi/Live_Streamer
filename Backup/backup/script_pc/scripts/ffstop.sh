#! /bin/bash
cd ~/script/

while ps axg | grep -v grep | grep ffmpeg > /dev/null; do 
	killall ffmpeg
	sleep 1
done
sh /root/script/movelog.sh
echo Ffmpeg stoped | tee -a /root/script/script.log >> /dev/kmsg
exit 0
