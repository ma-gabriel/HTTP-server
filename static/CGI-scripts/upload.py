#!/usr/bin/env python3

import os
import sys
import cgi
import cgitb

cgitb.enable()  # Show traceback in browser on error

print("Content-Type: text/plain\r\n")

# Make sure upload dir exists
upload_dir = "./uploads"
if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

form = cgi.FieldStorage()

if "file" not in form:
    print("No file received.")
    sys.exit(0)

field_item = form["file"]

if field_item.filename:
    filename = os.path.basename(field_item.filename)
    filepath = os.path.join(upload_dir, filename)

    try:
        with open(filepath, "wb") as f:
            while True:
                chunk = field_item.file.read(1024 * 1024)  # 1MB chunks
                if not chunk:
                    break
                f.write(chunk)
        print(f"Uploaded file: {filename}")
    except Exception as e:
        print(f"Error saving file: {e}")
else:
    print("Field 'file' exists but has no filename.")
