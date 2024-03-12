
#!/usr/bin/python3

import sys, os
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from util import util
from show import file_list_util
import cgitb

cgitb.enable()
uploadDir = '/Users/tayou/Desktop/tayou/42_webserv/document/uploaded'
fileList = util.getFileList(uploadDir)
htmlStringList = file_list_util.getHTMLStringList(fileList)
buttonStringList = delete_util.getButtonStringList(fileList)
print("Content-Type: text/html\n")
htmlFile = open('/Users/tayou/Desktop/tayou/42_webserv/document/html/delete.html', 'r')
htmlContent = htmlFile.read()
htmlContent = htmlContent.format(fileList = htmlStringList)
print(htmlContent)