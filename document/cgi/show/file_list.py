#!/usr/bin/python3

import cgitb, file_list_util

cgitb.enable()
uploadDir = '/Users/tayou/Desktop/tayou/42_webserv/document/uploaded'
print("Content-Type: text/html\n")
htmlFile = open('/Users/tayou/Desktop/tayou/42_webserv/document/html/file_list.html', 'r')
htmlContent = htmlFile.read()
htmlContent = htmlContent.format(
	fileList = file_list_util.getFileList(uploadDir))
print(htmlContent)
