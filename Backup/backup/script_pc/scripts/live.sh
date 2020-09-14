#! /bin/bash

trap bashtrap INT
# bash trap function is executed when CTRL-C is pressed:
# bash prints message => Executing bash trap subrutine !
bashtrap()
{
	echo 'INT detected closing Streaming'  | tee -a /root/script/script.log >> /dev/kmsg
	sh /root/script/ffstop.sh
	STATE=0
        exit 0
}

STATE=1
bitrate=1
frame_no=0
frame_no_prev=0
output=1
upLmt=2500
loLmt=100	
sh /root/script/ffstart.sh

echo 'extStr started' | tee -a /root/script/script.log >> /dev/kmsg
until [  $STATE -eq 0 ]; do							# -eq -lt -gt -le -ge
	sleep 7 
	################################## Camera Check #####################################
	if [ ! -e /root/script/output.mkv ]; then
		sh /root/script/ffstop.sh
		until [ -e /root/script/output.mkv ]; do		
			echo 'video mkv not available' | tee -a /root/script/script.log >> /dev/kmsg
			sleep 1
		done	
		(sh /root/script/ffstop.sh)&	
		(sh /root/script/ffstart.sh)&		
	fi
	################################## Bitrate Check #####################################
	output=$(tac /root/script/ffmp* | grep -m1 -o -E 'bitrate=[ ]{0,}+[0-9]{1,6}')
	bitrate=$(echo $output | grep -o -E '[0-9]{1,6}') 
	
	if [ -e $bitrate ]; then 
		bitrate=1 
	fi
	
	echo '********* Bitrate='$bitrate | tee -a /root/script/script.log >> /dev/kmsg
	if [ $upLmt -lt $bitrate ] ||  [ $loLmt -gt $bitrate ] ; then		
		(sh /root/script/ffstop.sh)&	
		(sh /root/script/ffstart.sh)&
		echo '********* High-Low Datarate ======> ffmpeg restarted' | tee -a /root/script/script.log >> /dev/kmsg		
	fi
	################################## Frame Check #####################################
	output=$(tac /root/script/ffmp* | grep -m1 -o -E 'frame=[ ]{0,}+[0-9]{1,}')
	frame_no=$(echo $output | grep -o -E '[0-9]{1,}') 
	
	if [ -e $frame_no ]; then 
		frame_no=1 
	fi
	
	echo '********* Frame No='$frame_no  | tee -a /root/script/script.log >> /dev/kmsg

	if [ $frame_no_prev -eq $frame_no ]; then
		(sh /root/script/ffstop.sh)&	
		(sh /root/script/ffstart.sh)&
		echo '********* Same Frame ======> ffmpeg restarted' | tee -a /root/script/script.log >> /dev/kmsg
	fi
	frame_no_prev=$frame_no

	################################## Time Check #####################################
	output=$(tac /root/script/ffmp* | grep -m1 -o -E 'time=[0-9]{1,}+[:]{1,}+[0-9]{1,}+[:]{1,}+[0-9]{1,}+[:]{1,}')
	frame_no=$(echo $output | grep -o -E '[0-9]{1,}') 
	
	if [ -e $frame_no ]; then 
		frame_no=1 
	fi
	
	echo '********* Frame No='$frame_no  | tee -a /root/script/script.log >> /dev/kmsg

	if [ $frame_no_prev -eq $frame_no ]; then
		(sh /root/script/ffstop.sh)&	
		(sh /root/script/ffstart.sh)&
		echo '********* Same Frame ======> ffmpeg restarted' | tee -a /root/script/script.log >> /dev/kmsg
	fi
	

done
sh /root/script/ffstop.sh
exit 0
