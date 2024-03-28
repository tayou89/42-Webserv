def makeHTMLFileString(fileName):
	htmlFileString = '<li><a href="/show/file.py?file={file}">{file}</a></li>'.format(
		file = fileName)
	return (htmlFileString)

def getHTMLStringList(fileList):
	htmlStringList = ''
	for file in fileList:
		htmlStringList += makeHTMLFileString(file)
	return (htmlStringList)
