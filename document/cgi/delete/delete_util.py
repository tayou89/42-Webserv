def	getDeleteStringList(fileList):
	buttonStringList = ''
	for fileName in fileList:
		buttonStringList +=  "<li><span><a href='/show/file.py?file={}'>{}</a></span><button onclick=\"deleteFile('{}')\">delete</button></li>".format(fileName, fileName, fileName)
	return (buttonStringList)