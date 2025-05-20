#!/usr/bin/env bash

for n in $(ls tests | wc -l); do
	exec ./tests/$n.test | nc localhost 8080
done
