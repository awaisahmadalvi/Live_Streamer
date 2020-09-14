#!/bin/bash


# V=$(cat test.json | python -c 'import json,sys;obj=json.load(sys.stdin);print obj["version"]')
# nc -k -l 4444 
# echo "Hello World" >/dev/tcp/103.226.217.136/4444

exec 5<>/dev/tcp/127.0.0.1/4444

echo $(cat test.json) >&5

read -e reply <&5

var1=$(echo $reply | cut -f1 -d$'\n')
echo $var1 > receive.json

V=$(cat receive.json | python -c 'import json,sys;obj=json.load(sys.stdin);print obj["status"]')
echo $V
a="available"

if [ "$a" = "$V" ]; then

	echo "{ \"streamId\": \"myStream\",\"action\": \"getaddr\"}" >&5
	
	read -e reply <&5
	var1=$(echo $reply | cut -f1 -d$'\n')
	echo $var1 > receive.json
	echo $var1
	V=$(cat receive.json | python -c 'import json,sys;obj=json.load(sys.stdin);print obj["addr"]')
	echo $V
	addr = "http://103.226.217.136/img/" | $V
	echo $addr
fi

exec 5>&-
exec 5<&-

exit
