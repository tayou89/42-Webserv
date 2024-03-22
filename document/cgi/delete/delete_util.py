def	getDeleteStringList(fileList):
	buttonStringList = ''
	for fileName in fileList:
		buttonStringList +=  "<li><span id='filename'><a href='/show/file.py?file={}'>{}</a></span><span id='button'><button onclick=\"deleteFile('{}')\">delete</button></span></li>".format(fileName, fileName, fileName)
	return (buttonStringList)