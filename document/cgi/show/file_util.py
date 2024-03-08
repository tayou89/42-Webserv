import os, cgitb

cgitb.enable()

def getFileList(dirPath):
	files = os.listdir(dirPath)
	listString = ''
	for file in files:
		listString += '<li><a href="/show/file.py?file={fileName}">{fileName}</a></li>'.format(fileName = file)
	return listString

import mimetypes

def getMIMEType(fileName):
	mimeType, _ = mimetypes.guess_type(fileName)
	return mimeType

