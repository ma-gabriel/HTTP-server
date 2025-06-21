import os
import sys

print("Status: 200 OK\r")
print("Content-Type: text\r\n\r")


print("argc:", len(sys.argv))
print("\nargv:")
for arg in sys.argv:
    print(f"\t{arg}")

print("\nenvp:")
for key, value in os.environ.items():
    print(f"\t{key}={value}")