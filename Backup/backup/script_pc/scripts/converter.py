#!/usr/bin/env python2
import os, time, shutil, commands, subprocess, shlex, signal, datetime
from time import localtime, strftime
from threading import Timer

vdo=null

def convert():
	global vdo	
	command_line = "ffmpeg -i %s -f mp4 -qp 20 converted/%s.mp4 -y -report" %(vdo, vdo)
	args = shlex.split(command_line)
	procLocal = subprocess.Popen(args)

def getFile():
        global vdo
	files = os.listdir(os.curdir)
	for f in files:
		if (f.endswith("MKV")):
			try:
				vdo = f
				convert()
			except EnvironmentError:
				continue

def main():
        getFile()	
main()
#exit()
