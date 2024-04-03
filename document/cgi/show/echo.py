#!/usr/bin/env python3

import sys

# HTTP 헤더 출력
print("Content-Type: text/plain; charset=utf-8")
print()

# 표준 입력에서 데이터 읽기
# while True:
data = sys.stdin.read(10000)
#   if not data:
    # break
print(data)
