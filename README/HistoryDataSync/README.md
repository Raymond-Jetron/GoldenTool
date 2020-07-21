# 简介
HistoryDataSync，历史数据同步工具。

.exe的为windows版，.out的为linux版。

# 入门
```
 .\HistoryDataSync.exe -h
,-_/,.       .                 .-,--.      .        .---.
' |_|/ . ,-. |- ,-. ,-. . .    ' |   \ ,-. |- ,-.   \___  . . ,-. ,-.
 /| |  | `-. |  | | |   | |    , |   / ,-| |  ,-|       \ | | | | |
 `' `' ' `-' `' `-' '   `-|    `-^--'  `-^ `' `-^   `---' `-| ' ' `-'
                         /|                                /|
                        `-'                               `-'

App description
Usage: .\HistoryDataSync.exe [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n,--taskname TEXT REQUIRED task name (=main)
  --source_host_name TEXT REQUIRED
                              source host name (=127.0.0.1)
  --source_port INT           source port number (=6327)
  --source_user TEXT          source user name (=sa)
  --source_password TEXT      source pass word (=golden)
  --sink_host_name TEXT REQUIRED
                              sink host name (=127.0.0.1)
  --sink_port INT             sink port number (=6327)
  --sink_user TEXT            sink user name (=sa)
  --sink_password TEXT        sink pass word (=golden)
  -s,--start_time TEXT        start time (=now)
                              format:
                               "YYYY-MM-DD hh:mm:ss.ms"
                               "mintime" start time is min UTC time : 1970-01-01 00:00:00.000
                               "now" start time is local real time
                               Enclosed in single or double quotes.
  -e,--end_time TEXT          end time (=forever)
                              format:
                               "YYYY-MM-DD hh:mm:ss.ms"
                               "forever" end time is max UTC time
  --thread_count INT          thread count (=1)
  --points_dir TEXT REQUIRED  point file directory (*.csv)
  --output_points_prop        out put all points' property
  --print_log                 print log to console
  --log_level INT             log level (=2) as info
                               0.trace
                               1.debug
                               2.info
                               3.warn
                               4.err
                               5.critical
                               6.off
  --Attention                 # 注意：
                              #   1.可以传入带毫秒的时间范围
                              #   2.如果同步多个标签点，会自动分配到多个线程
```

```
./HistoryDataSync.out -h
,-_/,.       .                 .-,--.      .        .---.             
' |_|/ . ,-. |- ,-. ,-. . .    ' |   \ ,-. |- ,-.   \___  . . ,-. ,-. 
 /| |  | `-. |  | | |   | |    , |   / ,-| |  ,-|       \ | | | | |   
 `' `' ' `-' `' `-' '   `-|    `-^--'  `-^ `' `-^   `---' `-| ' ' `-' 
                         /|                                /|         
                        `-'                               `-'         

App description
Usage: ./HistoryDataSync.out [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n,--taskname TEXT REQUIRED task name (=main)
  --source_host_name TEXT REQUIRED
                              source host name (=127.0.0.1)
  --source_port INT           source port number (=6327)
  --source_user TEXT          source user name (=sa)
  --source_password TEXT      source pass word (=golden)
  --sink_host_name TEXT REQUIRED
                              sink host name (=127.0.0.1)
  --sink_port INT             sink port number (=6327)
  --sink_user TEXT            sink user name (=sa)
  --sink_password TEXT        sink pass word (=golden)
  -s,--start_time TEXT        start time (=now)
                              format:
                               "YYYY-MM-DD hh:mm:ss.ms"
                               "mintime" start time is min UTC time : 1970-01-01 00:00:00.000
                               "now" start time is local real time
                               Enclosed in single or double quotes.
  -e,--end_time TEXT          end time (=forever)
                              format:
                               "YYYY-MM-DD hh:mm:ss.ms"
                               "forever" end time is max UTC time
  --thread_count INT          thread count (=1)
  --points_dir TEXT REQUIRED  point file directory (*.csv)
  --output_points_prop        out put all points' property
  --print_log                 print log to console
  --log_level INT             log level (=2) as info
                               0.trace
                               1.debug
                               2.info
                               3.warn
                               4.err
                               5.critical
                               6.off
  --Attention                 Attention:
                              1.Time range with milliseconds can be passed in.
                              2.If multiple points are sync, they are automatically assigned to multiple sync threads.
```

# 测试
在shell脚本配置参数：
```
#!/bin/sh
#
./HistoryDataSync.out -n task1 --source_host_name 192.168.152.134 --sink_host_name 192.168.152.137 --start_time '2016-9-4 8:9:3' --end_time '2019-12-4 8:9:3' --thread_count 1 --log_level 0 --print_log --points_dir '/home/golden/projects/GoldenTool/points'
```
在命令行直接传入参数：
```
./HistoryDataSync.out -n task1 --source_host_name 192.168.152.134 --sink_host_name 192.168.152.137 --start_time '2016-9-4 8:9:3' --end_time '2019-12-4 8:9:3' --thread_count 1 --log_level 0 --print_log --points_dir '/home/golden/projects/GoldenTool/points'
```

在bat脚本配置参数：

```
@echo off
cd /d %~dp0
REM 不输出点表
.\HistoryDataSync.exe -n task1 --source_host_name 192.168.152.134 --sink_host_name 192.168.152.137 --start_time "2016-9-4 8:9:3" --end_time "2019-12-4 8:9:3" --thread_count 1 --log_level 0 --print_log --points_dir "D:\Code\GoldenHistoryDataToRTDB\Debug_v30\points"
REM 输出点表
REM .\HistoryDataSync.exe -n task1 --source_host_name 192.168.152.134 --sink_host_name 192.168.152.137 --start_time "2016-9-4 8:9:3" --end_time "2019-12-4 8:9:3" --thread_count 1 --log_level 0 --print_log --points_dir "D:\Code\GoldenHistoryDataToRTDB\Debug_v30\points" --output_points_prop
```