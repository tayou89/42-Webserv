import os, cgitb

cgitb.enable()

def getFileList(dirPath):
	files = os.listdir(dirPath)
	listString = ''
	for file in files:
		listString += '<li><a href="{filePath}">{fileName}</a></li>'.format(
			filePath = dirPath + '/' + file, fileName = file)
	return listString
