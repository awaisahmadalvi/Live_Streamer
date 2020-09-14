#!/usr/bin/env python2
import os, shutil, commands, subprocess

global dotFile

def convert():
	global dotFile	
	command_line = "dot -Tpng  \"%s\"  >  \"%s\".png" %(dotFile, dotFile)
	os.system(command_line)
	command_line = "rm  \"%s\" " %(dotFile)
	os.system(command_line)

def getFile():
        global dotFile
	files = os.listdir(os.curdir)
	for f in files:
		if (f.endswith(".dot")):
			try:
				dotFile = f
				convert()
			except EnvironmentError:
				continue

def main():
        getFile()

main()
