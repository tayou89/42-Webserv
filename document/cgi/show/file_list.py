#!/usr/bin/python3

import sys, os
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from util import util
import cgitb, file_list_util

cgitb.enable()
uploadDir = '/Users/jinhyeop/Desktop/project/webserv/document/uploaded'
# uploadDir = '/Users/tayou/Desktop/tayou/42_webserv/document/uploaded'
# uploadDir = '/Users/yejlee2/42cursus/webserv/document/uploaded'
fileList = util.getFileList(uploadDir)
htmlStringList = file_list_util.getHTMLStringList(fileList)
print("Content-Type: text/html\r\n\r\n", end="")
htmlFile = open('/Users/jinhyeop/Desktop/project/webserv/document/html/file_list.html', 'r')
# htmlFile = open('/Users/tayou/Desktop/tayou/42_webserv/document/html/file_list.html', 'r')
# htmlFile = open('/Users/yejlee2/42cursus/webserv/document/html/file_list.html', 'r')
htmlContent = htmlFile.read()
htmlContent = htmlContent.format(fileList = htmlStringList)
print(htmlContent)
