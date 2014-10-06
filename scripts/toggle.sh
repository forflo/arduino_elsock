#!/bin/bash
HOST="http://141.60.125.177"

while :; do
	for i in {a..j}; do
	echo Toggle $i
	curl ${HOST}/$i > /dev/null 2>&1
	sleep 0.2
	done
done
