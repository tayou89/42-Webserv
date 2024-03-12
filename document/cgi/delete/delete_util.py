def	getButtonStringList(fileList):
	buttonStringList = ''
	for fileName in fileList:
		buttonStringList +=  "<button onclick=\"doAction(" + fileName + ")\">delete</button>"
	return (buttonStringList)