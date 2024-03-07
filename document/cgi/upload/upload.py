#!/usr/bin/python3

import os, cgi, cgitb, sys, html, upload_util

cgitb.enable()

print("Content-Type: text/html\n")

form = upload_util.getFormData()
pathInfo = upload_util.getPathInfo()
fileToUpload = upload_util.getFileData(form)
uploadedFile = upload_util.createUploadedFile(fileToUpload, pathInfo)
upload_util.copyFile(fileToUpload, uploadedFile)

print(f"The file {os.path.basename(fileToUpload.filename)} has been uploaded.")