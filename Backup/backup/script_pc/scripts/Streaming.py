#!/usr/bin/env python2
import os, time, shutil, commands, subprocess, shlex, signal, datetime
from time import localtime, strftime
from threading import Timer

now=None
STATE=True
procLive=None
procLocal=None
maxFault=2
faultCount=0
prevFrameNo=0
currVideoTime="00:00:00.00"
device="video2"

def startLocal():
	global now
	global procLocal
	now=strftime("%Y%m%d_%H%M%S", localtime())
	addLog(now)
	os.chdir("/root/gstreamer_build/bin")	
	command_line = "./gst-launch v4l2src device=/dev/%s ! video/x-h264, framerate=30/1, width=640, height=480 ! mpegtsmux ! filesink append=true location=/root/script/%s.avi" %(device, now)
	args = shlex.split(command_line)
	procLocal = subprocess.Popen(args)
	print procLocal.communicate()
	addLog("Local started")
	checkVideoFile()
	restartLive()

def startLive():
	global now
	global procLive
	global prevFrameNo
	global currVideoTime
	moveLog()
	os.chdir("/root/script/logs")
	command_line = "./ffmpeg -re -ss %s -r 10 -i /root/script/%s.avi -filter:v scale=320:-1 -b:v 1500k -threads 20 -f flv rtmp://103.226.217.136/live/myStream -report" %(currVideoTime, now)
	args = shlex.split(command_line)
	procLive = subprocess.Popen(args)
	addLog("Live started")
	time.sleep(3)
	if(invalidData()):
		addLog("invalid input file")
		startLive()
	time.sleep(10)
	checkLiveStatus()

def moveLog():
	os.chdir("/root/script/logs/")	
	files = os.listdir("/root/script/logs/")
	for f in files:
		if (f.startswith("ffmpeg-")):
			try:
				shutil.move(f, "/root/script/logs/live/")
			except EnvironmentError:
				continue
				
	addLog("logs moved")

def checkVideoFile():
	global now
	count=0
	while (os.path.isfile("/root/script/%s.avi" %now) == False):
		if (count > 5):
		      restartLocal()
		      count=0
		time.sleep(2)
		addLog("waiting for video file")
		count = count+1
	addLog("Local file Started, Ready to go live")

def addLog(strLog):
	log = open("/root/script/script.log", 'a+')
	log.write(strLog+"\n")
	log.close()
	print strLog

def invalidData():
	o = commands.getstatusoutput("tac /root/script/logs/ffmp*.log | grep 'Invalid data found when processing input'")
	if(o[0]==0):
		return True
	return False

def getBitrate():
	o = commands.getstatusoutput("tac /root/script/logs/ffmp*.log | grep -m1 -o -E 'bitrate=[ ]{0,}+[0-9]{1,6}'")
	a = o[1].split('\n')
	br = a[0].strip('bitrate= ')
	return br

def getFrameNo():
	o = commands.getstatusoutput("tac /root/script/logs/ffmp*.log | grep -m1 -o -E 'frame=[ ]{0,}+[0-9]{1,}'")
	a = o[1].split('\n')
	fn = a[0].strip('frame= ')
	return fn

def getVideoTime():
	o = commands.getstatusoutput("tac /root/script/logs/ffmp*.log | grep -m1 -o -E 'time=[0-9]{1,}+[:]+[0-9]{1,}+[:]+[0-9]{1,}+[.]+[0-9]{1,}'")
	if (o[0] != 0):
		global currVideoTime
		return currVideoTime
	a = o[1].split('\n')
	x = ("echo %s | grep -o -E '[0-9]{1,}+[:]+[0-9]{1,}+[:]+[0-9]{1,}+[.]+[0-9]{1,}'" % a[0])
	vt = commands.getstatusoutput(x)
	print vt
	timeNow = addTime(vt[1],currVideoTime)
	return timeNow

def checkLiveStatus():
	global STATE
	global faultCount
	global maxFault
	if (STATE):
		t = Timer(3, checkLiveStatus).start()
	else:
		return
	upLmt=2500
	loLmt=100
	frameNo=0
	bitrate=1
	global prevFrameNo
	bitrate = getBitrate()
	
	addLog("Datarate: %s" %bitrate)
	try:
		bitrate = int(bitrate)
	except ValueError:
		addLog("Error in bitrate")
		#bitrate=  #something
	if(bitrate < loLmt):
		if(faultCount>=maxFault):
			addLog("Low Datarate ======> ffmpeg restarted")
			faultCount = 0
			restartLive()
		faultCount = faultCount + 1
		return
	if(bitrate > upLmt):

		if(faultCount>=maxFault):
			addLog("High Datarate ======> ffmpeg restarted")
			faultCount = 0
			restartLive()
		faultCount = faultCount + 1
		return
	
	frameNo = getFrameNo()
	addLog("FrameNo: %s" %frameNo)
	if(prevFrameNo == frameNo):
		if(faultCount>=maxFault-1):
			addLog("Same Frame ======> ffmpeg restarted")
			faultCount = 0
			restartLive()
		faultCount = faultCount + 1
		return
	prevFrameNo = frameNo
	
def restartLive():
	global procLive
	global currVideoTime
	print "1. time" + str(currVideoTime)
	currVideoTime = getVideoTime()
	print "2. time" + str(currVideoTime)
	if(procLive != None):
		print procLive.pid
		procLive.terminate()
		addLog("Live Terminate")
		time.sleep(1)
	procLive = startLive()

def restartLocal():
	global procLocal
	print procLocal.pid
	procLocal.terminate()
	addLog("Local Terminate")
	time.sleep(1)
	procLocal = startLocal()

def addTime(tStrPrev,tStrNow):
	try:
		tPrev = datetime.datetime.strptime(tStrPrev, "%H:%M:%S.%f")
	except ValueError:
		tPrev = datetime.datetime.strptime(tStrPrev, "%H:%M:%S")
	try:
		tNow = datetime.datetime.strptime(tStrNow, "%H:%M:%S.%f")
	except ValueError:
		tNow = datetime.datetime.strptime(tStrNow, "%H:%M:%S")
	
	dtPrev = datetime.timedelta(minutes=tPrev.minute, seconds=tPrev.second, microseconds=tPrev.microsecond)
	dtNow = datetime.timedelta(minutes=tNow.minute, seconds=tNow.second, microseconds=tNow.microsecond)
	totalTime = dtPrev + dtNow
	return str(totalTime)

def sigIntHandler(signum, frame):
	print 'Signal handler called with signal ', signum
	global procLive
	global procLocal
	global STATE
	if(procLive != None):
		procLive.terminate()
	if(procLocal != None):
		procLocal.terminate()
	STATE=False
	commands.getstatusoutput(".././killallproc")
	exit()

def checkCamera():
	return os.path.exists("/dev/video0")|os.path.exists("/dev/video1")

def main():
	signal.signal(signal.SIGINT, sigIntHandler)
	moveLog()
	while(checkCamera() == False):
		addLog("Camera not available.")
		time.sleep(1)
	print checkCamera()
	startLocal()	
	print "done"

main()
#exit()

