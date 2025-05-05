#!/usr/bin/env bash

printf "GET / HTTP/1.0\r\nHost: localhost:8080\r\n" | nc localhost 8080 
