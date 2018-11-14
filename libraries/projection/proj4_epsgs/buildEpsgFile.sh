#!/bin/bash

#i=0
#while read LINE
#do
#    data[$i]=$LINE
#    i=$(expr $i + 1)
#done < epsg_esri.list

ifs_old=$IFS
glob_old=$GLOBIGNORE

IFS=$'\r\n' GLOBIGNORE='*' 
data=($(cat epsg.list))

IFS=$ifs_old
GLOBIGNORE=$glob_old

len=${#data[@]}

for((i=1;i<$len;i++))
do
    i1=$(expr $i + 1)

    #l1=$(echo ${data[$i]} | cut -c1 )
    l2=$(echo ${data[$i1]} | cut -c1 )
    if [ $l2 == "#" ] ; then
        continue;
    fi

    cs_desc=$(echo ${data[$i]} | cut -d\# -f2)
    cs_epsg=$(echo ${data[$i1]} | cut -d\< -f2 | cut -d\> -f1)
    cs_init=$(echo ${data[$i1]} | cut -d\> -f2 | cut -d\< -f1)

    cs_init=$(echo $cs_init)
    cs_desc=$(echo $cs_desc)

    pct=$(printf "%6.2f%%" $(echo "scale=2;( ( $i / 2.0 ) / ( $len / 2.0 ) ) * 100.0 " | bc))

    (>&2 echo "[$pct] $cs_epsg")
    echo $cs_epsg,"this->m_epsgMapping.insert(pair<int,pair<string,string>>("$cs_epsg",pair<string,string>("\"$cs_init\"","\"$cs_desc\"")));"

    i=$i1
done
