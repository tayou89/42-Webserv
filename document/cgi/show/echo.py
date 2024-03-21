#!/usr/bin/env python3

import sys

# HTTP 헤더 출력
print("Content-Type: text/plain;charset=utf-8")
print()

# 표준 입력에서 데이터 읽기
data = sys.stdin.read()

# 읽은 데이터를 표준 출력으로 내보내기
print(data)
