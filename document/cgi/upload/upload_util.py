import sys, cgi, os, html

def getFormData():
	form = cgi.FieldStorage()
	if not "fileToUpload" in form:
		print("Error couldn't find file to upload")
		sys.exit(0)
	return form

def getPathInfo():
	path_info = os.getenv('PATH_INFO')
	if path_info:
		print(f"PATH_INFO: {path_info}<br>")
	else:
		print("PATH_INFO does not exist<br>")
		sys.exit(0)
	return path_info

def	getFileData(form):
	file = form["fileToUpload"]
	if not file.filename:
		print("Error: can't find file to upload<br>")
		sys.exit(0)
	return file

def createUploadedFile(uploadFile, uploadPath):
	file_name = os.path.basename(uploadFile.filename)
	filePath = uploadPath + '/' + html.escape(uploadFile.filename)
	uploadedFile = open(filePath, 'wb')
	return uploadedFile

def copyFile(origin, copy):
	chunk = origin.file.read(100000)
	while chunk:
		copy.write(chunk)
		chunk = origin.file.read(100000)