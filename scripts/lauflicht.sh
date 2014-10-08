#!/bin/bash
PORTNUM=10
CNT=0

while :; do
	echo toggle all $CNT
	((CNT++))
	for i in {a..j}; do
		curl http://$IP/$imooo > /dev/null 2>&1
	done
done
