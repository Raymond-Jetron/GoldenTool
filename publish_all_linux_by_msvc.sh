#!/bin/sh
#cpu:x64,ARM,ARM64,MIPS
cpu_type=x64

#publish
./publish_app_linux.sh CreatePoint $cpu_type
./publish_app_linux.sh MakeData $cpu_type
./publish_app_linux.sh QueryData $cpu_type
./publish_app_linux.sh HistoryDataSync $cpu_type