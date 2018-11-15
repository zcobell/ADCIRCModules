#!/bin/bash

ifs_old=$IFS
glob_old=$GLOBIGNORE

IFS=$'\r\n' GLOBIGNORE='*' 
data=($(cat list.txt))

#IFS=$ifs_old
#GLOBIGNORE=$glob_old

len=${#data[@]}

echo $len

rm epsgVar.cpp
rm epsgDesc.cpp
rm epsgInit.cpp

i=0
for line in ${data[@]}
do
    epsg=$(echo $line | cut -d, -f1)
    desc=$(echo $line | cut -d\" -f4)
    init=$(echo $line | cut -d\" -f2)
    echo "{"$epsg","$i"}," >> epsgVar.cpp
    echo \"$desc\"","  >> epsgDesc.cpp
    echo \"$init\""," >> epsgInit.cpp
    i=$(expr $i + 1)
done



