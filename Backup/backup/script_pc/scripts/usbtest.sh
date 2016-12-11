#! /bin/sh

echo working >> /root/script/script.log	

if [ "${ACTION}" = "add" ]; then #&& [ -f "${DEVICE}" ]; then
    echo ${DEVICE} >> /root/script/script.log	
fi
