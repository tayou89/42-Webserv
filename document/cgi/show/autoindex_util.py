import sys, os
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from util import util

def getFilePathList(fileNameList, directoryPath):
	filePathList = []
	for fileName in fileNameList:
		filePathList.append(directoryPath + '/' + fileName)
	return (filePathList)

def getFileTimeList(filePathList):
	fileTimeList = []
	for filePath in filePathList:
		fileTimeList.append(util.getFileModifiedTimeString(filePath))
