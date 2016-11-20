#!/bin/bash

mkdir /root/update
cd /root/update
wget $1
tar -xJf UpdatePackage.tar.xz
rm -rf UpdatePackage.tar.xz
sh update.sh
exit
