#!/usr/bin/python3

import os, cgi, cgitb, sys, html

cgitb.enable()

print("Content-Type: text/html\n")

form = cgi.FieldStorage()

if not "fileToUpload" in form:
	print("Error couldn't find file to upload")
	sys.exit(1)
path_info = os.getenv('PATH_INFO')
if path_info:
	print(f"PATH_INFO: {path_info}<br>")
else:
	print("PATH_INFO does not exist<br>")
	sys.exit(0)

file = form["fileToUpload"]

if not file.filename:
	print("Error: can't find file to upload<br>")
	sys.exit(0)

file_name = os.path.basename(file.filename)
target_file = path_info + '/' + html.escape(file.filename)

fout = open(target_file, 'wb')
chunk = file.file.read(100000)
while chunk:
	fout.write(chunk)
	chunk = file.file.read(100000)
print(f"The file {os.path.basename(file.filename)} has been uploaded.")