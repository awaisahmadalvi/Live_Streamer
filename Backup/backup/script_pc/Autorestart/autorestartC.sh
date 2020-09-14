#!/bin/bash 
# See how we were called. {start|stop}

WMCOMMAND=${1}

start() {
	echo "Start up script started on " $(date) >> /dev/kmsg
	exitcodeOTA=-1
	(until [ $exitcodeOTA -eq 0 ]; do

		OTA_log="/root/OTA/logs/log-`date +'%Y-%m-%d_%H-%M-%S'`"
		[ -f "$OTA_log" ] || touch "$OTA_log"
		cd /root/OTA/
		echo 'Starting OTA Server...'
		stdbuf -o 0 /root/OTA/./OTAServer.o | tee "$OTA_log" >> /dev/kmsg
		exitcodeOTA=${PIPESTATUS[0]}
		echo "OTAServer crashed, restarting now @ " $(date) >> /dev/kmsg
		sleep 1

	done)&
	exitcodeStrm=-1
	(until [ $exitcodeStrm -eq 0 ]; do

		Streamer_log="/root/Streamer/logs/log-`date +'%Y-%m-%d_%H-%M-%S'`"
		[ -f "$Streamer_log" ] || touch "$Streamer_log"
		cd /root/Streamer/
		echo 'Starting OTA Server...'
		stdbuf -o 0 /root/Streamer/./StreamerServer.o | tee "$Streamer_log" >> /dev/kmsg
		exitcodeStrm=${PIPESTATUS[0]}
		echo "StreamerServer crashed, restarting now @ " $(date) >> /dev/kmsg
		sleep 1 

	done)&
}
stop() {
	killall /root/Streamer/StreamerServer.o 
	killall	/root/OTA/OTAServer.o
	killall autorestartC.sh
}


case "$WMCOMMAND" in
start)
	start
	;;
stop)
	echo -n -e $"\t$WMSBASE_NAME: stopping...\n"
	stop
	;;
*)
   	echo -n -e $"\tUsage: $WMSBASE_NAME {start|stop}\n"
	exit 1
esac

exit 0

