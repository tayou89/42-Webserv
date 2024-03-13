def	getDeleteStringList(fileList):
	buttonStringList = ''
	for fileName in fileList:
		buttonStringList +=  "<li><span><a href='/show/file.py?file={}'>{}</a></span><button onclick=\"doAction('{}')\">delete</button></li>".format(fileName, fileName, fileName)
	return (buttonStringList)