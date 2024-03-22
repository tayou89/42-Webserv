#!/usr/bin/python3
import cgi
import os
import urllib.parse

print("Content-Type: text/plain\n")

# 쿼리 스트링에서 파일 이름을 수동으로 파싱합니다.
query_string = os.getenv('QUERY_STRING', '')
print(f"query_string: {query_string}")
query_params = urllib.parse.parse_qs(query_string)
filename = query_params.get('filename', [None])[0]

if not filename:
    print("Error: No filename provided.")
else:
    file_path = os.path.join("/Users/jinhyeop/Desktop/project/webserv/document/uploaded", filename)
    print(f"{file_path}")
    if os.path.exists(file_path):
        try:
            os.remove(file_path)
            print(f"Success: {filename} has been deleted.")
        except Exception as e:
            print(f"Error: {str(e)}")
    else:
        print("Error: File not found.")
