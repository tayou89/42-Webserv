#!/usr/bin/python3

import sys, os
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from util import util
from show import file_list_util
import cgitb, autoindex_util, sys

cgitb.enable()

print("Content-Type: text/html\n")
try:
	autoindexDirectory = util.getENV('DOCUMENT_ROOT')
	fileNameList = util.getFileList(autoindexDirectory)
	htmlFileNameList = file_list_util.getHTMLStringList(fileNameList)
	filePathList = autoindex_util.getFilePathList(fileNameList, autoindexDirectory)
	fileTimeList = autoindex_util.getFileTimeList(filePathList)
except Exception as e:
	print("Error: ", e, file = sys.stderr)