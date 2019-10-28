#简介
GoldenTool为庚顿跨平台工具包，具体工具包含：
1、HistoryDataSync历史数据同步

#入门
1.	HistoryDataSync
```
.\HistoryDataSyncW.exe -h
,-_/,.       .                 .-,--.      .        .---.
' |_|/ . ,-. |- ,-. ,-. . .    ' |   \ ,-. |- ,-.   \___  . . ,-. ,-.
 /| |  | `-. |  | | |   | |    , |   / ,-| |  ,-|       \ | | | | |
 `' `' ' `-' `' `-' '   `-|    `-^--'  `-^ `' `-^   `---' `-| ' ' `-'
                         /|                                /|
                        `-'                               `-'

App description
Usage: D:\Code\Linux\GoldenTool\HistoryDataSyncW\bin\x64\Release\HistoryDataSyncW.exe [OPTIONS]

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

#生成与测试
VS2017

