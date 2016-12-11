#! /bin/bash
sh ffstop.sh

(ffmpeg -f v4l2 -video_size 640x480 -framerate 10 -i /dev/video0 -f flv -threads 2 rtmp://103.226.217.136/live/myStream -report)&

exit 0


trap bashtrap INT
# bash trap function is executed when CTRL-C is pressed:
# bash prints message => Executing bash trap subrutine !
bashtrap()
{
	echo 'INT detected closing Streaming' >> /dev/kmsg
	STATE=0
        exit 0
}


