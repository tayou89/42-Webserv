import os, cgitb

cgitb.enable()

def getFileList(dirPath):
	files = os.listdir(dirPath)
	return (files)

import mimetypes

def getMIMEType(fileName):
	mimeType, _ = mimetypes.guess_type(fileName)
	return (mimeType)

def getENV(envName):
	value = os.getenv(envName)
	if value is None:
		raise Exception('env ' + envName + ' doesn\'t exist')
	return (value)
