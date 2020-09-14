#! /bin/bash
cd /root/script/
mv /root/script/ffmp*.log /root/script/logs/live/
echo logs moved | tee -a /root/script/script.log >> /dev/kmsg
exit 0
