#!/bin/bash
HOST="http://141.60.125.177"

while :; do
	temp=""
	for i in {c..j}; do
		curl ${HOST}/p?L$temp=0"&"L$i=1 > /dev/null 
		temp="c"
		sleep 0.2
	done
done
