#! /bin/bash

output=$(sudo iwlist wlan0 scan | grep ESSID | grep -o '"[^"]\+"')
IFS=' '

ELEMENTS=${#output[@]}
for (( i=0;i<$ELEMENTS;i++)); do
    echo $i${output[${i}]}
done 

#nmcli d wifi connect Hackers password 03454108764 iface wlan0

#nmcli d disconnect iface wlan0

