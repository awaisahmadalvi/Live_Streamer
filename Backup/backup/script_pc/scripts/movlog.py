#!/usr/bin/env python2
import os, time, shutil, commands, subprocess, shlex, signal, datetime, string
from time import localtime, strftime
from threading import Timer

def moveLog():
	
	os.chdir("/root/script/logs/")	
	now=strftime("%Y%m%d", localtime())
	print now
	try:
		os.mkdir(now)
	except OSError:
		print "already Exist"
	dirPath = "/root/script/logs/{0}/".format(now)
	files = os.listdir("/root/script/logs/")
	for f in files:
		if (f.startswith("ffmpeg-")):
			try:
				shutil.move(f, dirPath)
			except EnvironmentError:
				os.remove(f)                          
				continue
				
	addLog("logs moved")
moveLog()
