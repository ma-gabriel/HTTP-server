uoaefmuhzrfhazùf

#!/usr/bin/env python3

import os
import sys
from http import cookies

def main():
    # Parse incoming cookies from HTTP_COOKIE environment variable
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    cookie_jar = cookies.SimpleCookie()
    cookie_jar.load(cookie_header)

    # Print HTTP headers
    # We'll set a new cookie named "user" with value "chatgpt"
    new_cookie = cookies.SimpleCookie()
    new_cookie['user'] = 'chatgpt'
    new_cookie['user']['path'] = '/'
    new_cookie['user']['max-age'] = 3600  # 1 hour

    # Output HTTP headers
    print("Content-Type: text/html")
    for morsel in new_cookie.values():
        print("Set-Cookie: {}".format(morsel.OutputString()))
    print()  # blank line ends headers

    # Output HTML body
    print("<html><body>")
    print("<h1>Cookie Demo</h1>")

    if cookie_jar:
        print("<p>Received cookies:</p><ul>")
        for key, morsel in cookie_jar.items():
            print("<li><b>{}</b> = {}</li>".format(key, morsel.value))
        print("</ul>")
    else:
        print("<p>No cookies received.</p>")

    print("<p>Set a new cookie 'user=chatgpt' valid for 1 hour.</p>")
    print("</body></html>")

if __name__ == "__main__":
    main()
