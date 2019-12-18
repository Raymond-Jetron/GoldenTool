#!/bin/sh
#cpu:x64,ARM,ARM64,MIPS
cpu_type=x64

sed -i "s/^CPU_TYPE.*/CPU_TYPE = $cpu_type/g" ./build/qt/GoldenCreatePoint/GoldenCreatePoint.pro 
sed -i "s/^CPU_TYPE.*/CPU_TYPE = $cpu_type/g" ./build/qt/GoldenMakeData/GoldenMakeData.pro 
sed -i "s/^CPU_TYPE.*/CPU_TYPE = $cpu_type/g" ./build/qt/GoldenQueryData/GoldenQueryData.pro 
sed -i "s/^CPU_TYPE.*/CPU_TYPE = $cpu_type/g" ./build/qt/HistoryDataSync/HistoryDataSync.pro 

#run qmake
/opt/Qt5.4.0/5.4/gcc_64/bin/qmake ./GoldenTool.pro -r -spec linux-g++

#run make
make -f linux.Makefile

#publish
./publish_app_linux.sh GoldenCreatePoint $cpu_type
./publish_app_linux.sh GoldenMakeData $cpu_type
./publish_app_linux.sh GoldenQueryData $cpu_type
./publish_app_linux.sh HistoryDataSync $cpu_type