<<<<<<< HEAD
#!/bin/bash
HOST="http://141.60.125.177"

while :; do
	temp="j"
	for i in {c..j}; do
		echo $i on
		curl "${HOST}/q?L$temp=0&L$i=1" > /dev/null 2>&1
		echo $temp off
		temp=$i
	done
done
