#!/usr/bin/python3

import sys, os
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from util import util
import cgitb, delete_util

cgitb.enable()
uploadDir = '/Users/yejlee2/Desktop/yejlee2/42_webserv/document/uploaded'
fileList = util.getFileList(uploadDir)
buttonStringList = delete_util.getDeleteStringList(fileList)
print("Content-Type: text/html\n")
htmlFile = open('/Users/yejlee2/Desktop/yejlee2/42_webserv/document/html/delete.html', 'r')
htmlContent = htmlFile.read()
htmlContent = htmlContent.format(deleteButtonList = buttonStringList)
print(htmlContent)