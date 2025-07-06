#!/bin/sh
filesdir=$1
searchstr=$2

if [ $# -ne 2 ] 
then 
    echo " user didnot enter the specified number of parameters "
    exit 1 
fi 

if [ -d "$filesdir" ] 
then 
    echo " file does exist"
else
    echo " filesdir does not represent a directory on the filesystem"
    exit 1
fi

Y=$(grep -rw "$filesdir" -e "$searchstr" | wc -l)
X=$(find $filesdir -type f | wc -l)

echo "The number of files are $X and the number of matching lines are $Y"