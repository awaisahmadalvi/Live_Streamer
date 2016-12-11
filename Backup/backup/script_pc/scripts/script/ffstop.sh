#! /bin/bash
while ps axg | grep -v grep | grep ffmpeg > /dev/null; do 
	killall ffmpeg
	sleep 1
done
if [ -e ffmp*.log ]; then 
	mv ffmp*.log logs/
	echo logs moved
fi
exit 0

