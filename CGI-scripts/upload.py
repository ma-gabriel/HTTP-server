#!/usr/bin/env python3
import sys
import time

print("Content-Type: text/plain\n")
body = sys.stdin.read()
print("STDIN length:", len(body))
print("First 200 chars:\n", body[:200])
