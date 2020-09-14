#!/bin/bash

nc -k -l 4444

read -u $var -r msg_in
echo "msg_in: $msg_in"


echo "asd" >$var 

