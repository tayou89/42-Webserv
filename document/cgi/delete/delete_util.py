def	getButtonStringList(fileList):
	buttonStringList = ''
	for fileName in fileList:
		buttonStringList +=  "<button onclick=\"doAction('{}')\">delete</button>".format(fileName)
	return (buttonStringList)