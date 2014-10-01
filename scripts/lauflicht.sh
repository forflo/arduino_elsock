#!/bin/bash

while :; do
	curl http://141.60.125.177/9 > /dev/null 2>&1
	echo toggle 9
	sleep 0.1
	curl http://141.60.125.177/8 > /dev/null 2>&1
	echo toggle 8
	sleep 0.1
	curl http://141.60.125.177/7 > /dev/null 2>&1
	echo toggle 7
	sleep 0.1
done
