#!/bin/sh
#publish app name
appname=$1   
#cpu
cpu_type=$2
echo "Publish "$appname

#app path
bin=./bin/$appname/$cpu_type/Release/$appname
echo "App path : "$bin

#publish dir
desDir=./publish/$appname.$cpu_type.$(date "+%Y%m%d%H%M%S")
#make publish
if [ ! -d $desDir ];then
      #echo "makedir $desDir"
      mkdir $desDir
fi 

#libList=$(ldd $bin | awk  '{if (match($3,"/")){ printf("%s "),$3 } }')
#cp $libList $desDir
cp $bin $desDir
cp ./third/api/$cpu_type/lib* $desDir