#!/bin/sh
writefile=$1
writestr=$2

if [ $# -ne 2 ] 
then 
    echo " user didnot enter the specified number of parameters "
    exit 1 
else 
    echo " success number of args entered by user "
fi 

dir="$(dirname "$writefile")"
# file="$(basename "$writefile")"
mkdir -p $dir
touch $writefile
echo ${writestr} > ${writefile} 