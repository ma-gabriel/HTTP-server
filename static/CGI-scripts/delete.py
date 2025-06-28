#!/usr/bin/env python3
import cgi
import os
import urllib.parse
import sys

print("Content-Type: text/plain\r")

method = os.environ.get("REQUEST_METHOD", "")
if method != "DELETE":
    print("Status: 405 Method Not Allowed\r")
    print("405 Method Not Allowed")
    sys.exit(0)
upload_dir = "./uploads"

query = os.environ.get("QUERY_STRING", "")
params = urllib.parse.parse_qs(query)

filename = params.get("filename", [None])[0]


if not filename:
    print("Status: 422 Unprocessable Content \r\n\r")
    print("No filename specified.")
    sys.exit(0)

filepath = os.path.join(upload_dir, filename)

if not os.path.exists(filepath):
    print("Status: 404 Not Found\r\n\r")
    print(f"File '{filename}' not found.")
elif not os.path.isfile(filepath):
    print("Status: 401 Unauthorized \r\n\r")
    print(f"'{filename}' is not a file.")
else:
    try:
        os.remove(filepath)
        print("\r\n\r")
        print(f"Deleted file: {filename}")
    except Exception as e:
        print("Status: 403 Forbidden \r\n\r")
        print(f"Error deleting file: {e}")
