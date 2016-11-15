#!/usr/bin/env python

import os
import platform

def isWindows():
    return "Windows" in platform.system()

def isLinux():
    return "Linux" in platform.system()

if isWindows():
	if not os.path.isdir(os.path.join(os.getcwd(), 'build.vc9')):
		os.mkdir("build.vc9")
	os.system("cd build.vc9 && cmake -G \"Visual Studio 9 2008\" ..")
	if not os.path.isdir(os.path.join(os.getcwd(), 'build.vc14')):
		os.mkdir("build.vc14")
	os.system("cd build.vc14 && cmake -G \"Visual Studio 14 2015\" ..")
else:
	if not os.path.isdir(os.path.join(os.getcwd(), 'build.gcc')):
		os.mkdir("build.gcc")
	os.system("cd build.gcc && cmake ..")

