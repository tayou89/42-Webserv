#!/usr/bin/python3

import cgitb, file_util

cgitb.enable()
uploadDir = '../../uploaded'
print("Content-Type: text/html\n")
htmlFile = open('../..//html/file_list.html', 'r')
htmlContent = htmlFile.read()
htmlContent = htmlContent.format(
	fileList = file_util.getFileList(uploadDir))
print(htmlContent)
