#!/usr/bin/python3

#import cgi, cgitb, os, file_util, sys
#
#cgitb.enable()
#
#form = cgi.FieldStorage()
#currentDirectory = os.getcwd()
#fileName = form.getvalue('file')
#filePath = '/Users/tayou/Desktop/tayou/42_webserv/document/uploaded' + fileName
#
#print(filePath)
#try:
#	if os.path.isfile(filePath):
#		mimeType = file_util.getMIMEType(fileName)
#		if mimeType is None:
#			mimeType = 'application/octet-stream'
#		file = open(filePath, 'rb') 
#		print(filePath)
#		sys.stdout.buffer.write(f"Content-Type: {mimeType}\n\n".encode('utf-8'))
#		sys.stdout.flush()
#		sys.stdout.buffer.write(file.read())
#	else:
#		print("Content-Type: text/html\n")
#except Exception as e:
#	print("Content-Type: text/html\n")
#	print()
import sys, os
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from util import util
import cgi, cgitb, os, sys

cgitb.enable()

form = cgi.FieldStorage()
fileName = form.getvalue('file')
filePath = '/Users/jinhyeop/Desktop/project/webserv/document/html/uploaded' + '/' + fileName

try:
    if os.path.isfile(filePath):
        mimeType = util.getMIMEType(fileName)
        if mimeType is None:
            mimeType = 'application/octet-stream'
        
        sys.stdout.buffer.write(f"Content-Type: {mimeType}\r\n\r\n".encode('utf-8'))
        with open(filePath, 'rb') as file:
            sys.stdout.flush()
            while True:
                chunk = file.read(1024)
                if not chunk:
                    break
                sys.stdout.buffer.write(chunk)
                # encoded_str = encoded_chunk.decode('utf-8')
                # sys.stderr.buffer.write(encoded_chunk)
                # print(encoded_chunk, file=sys.stderr)
                # sys.stdout.buffer.write(encoded_str)
                sys.stdout.flush()
    else:
        print("Content-Type: text/html\n")
        print("File not found.: {}".format(filePath))
except Exception as e:
    print("Content-Type: text/html\n")
    print(f"Error: {str(e)}")
