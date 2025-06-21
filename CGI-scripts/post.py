#!/usr/bin/env python3

import os
import sys

# Required HTTP header
print("Content-Type: text/plain\n")

# Get Content-Length from environment
length = os.environ.get("CONTENT_LENGTH")
if length:
    length = int(length)
    body = sys.stdin.read(length)
    print("Received POST data:")
    print(body)
else:
    print("No POST data received.")
