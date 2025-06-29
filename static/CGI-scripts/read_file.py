#!/usr/bin/env python3
import os
import cgi
import urllib.parse

print("Content-Type: text/plain\r")

query = os.environ.get("QUERY_STRING", "")
params = urllib.parse.parse_qs(query)

filename = params.get("filename", [None])[0]

if filename is None:
    print("Status: 422 Unprocessable Content \r\n\r")
    print("No filename specified.")
else:
    try:
        with open(f"./uploads/{filename}", "r") as f:
            content = f.read()
            print("Status: 200 OK\r\n\r")
            print(f"File: {filename}\nContent:\n{content}")
    except Exception as e:
        print("Status: 404 Not Found\r\n\r")
        print(f"Error reading file: {e}")
