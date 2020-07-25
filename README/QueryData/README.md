# 简介
QueryData，数据查询工具。
.exe的为windows版，.out的为linux版。

# 入门
```
 .\QueryData.exe -h
,,--.                    .-,--.      .
|`. | . . ,-. ,-. . .    ' |   \ ,-. |- ,-.
|  .| | | |-' |   | |    , |   / ,-| |  ,-|
`---\ `-^ `-' '   `-|    `-^--'  `-^ `' `-^
     `             /|
                  `-'

App description
Usage: .\QueryData.exe [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n,--taskname TEXT REQUIRED task name (=main)
  -a,--address TEXT ...       host name (=127.0.0.1)
                               You can set up multi-IP addresses separated by spaces to take advantage of the server's multi-network card.
                               e.g. -a 192.168.0.2 192.168.0.3 192.168.0.4 192.168.0.5
  -p,--port INT               port number (=6327)
  -u,--user TEXT              user name (=sa)
  -w,--password TEXT          pass word (=admin)
  -s,--starttime TEXT         start time (=now)
                              format:
                               "YYYY-MM-DD hh:mm:ss.ms"
                               "mintime" start time is min UTC time : 1970-01-01 00:00:00.000
                               "now" start time is local real time
                               Enclosed in single or double quotes.
  -e,--endtime TEXT           end time (=forever)
                              format:
                               "YYYY-MM-DD hh:mm:ss.ms"
                               "forever" end time is max UTC time
  --search TEXT               search condition
  --first_point INT           first point's id (=1)
  --point_count INT           point count (=1)
  --point_interval INT        point interval (=1)
  --query_mode TEXT           query mode (=history_archived)
                               1.history_archived
                               2.history_archived_ex
                               3.plot_value
                               4.interval_value
                               5.summary_value
                               6.group_by_interval_summary_value
                               7.scan_snapshot_value
                               8.subscribe_snapshot_value
  --query_batch_count INT     query values count per batch (=1000)
  --interval INT              query interval (=1000)
  --thread_count INT          thread count (=1)
  --print_log                 print log to console
  --log_level INT             log level (=2) as info
                               0.trace
                               1.debug
                               2.info
                               3.warn
                               4.err
                               5.critical
                               6.off
  --result_file TEXT          result file path, default is empty.
  --Attention                 # 注意：
                              #   1.可以传入带毫秒的时间范围
                              #   2.如果查询多个标签点，会自动分配到多个线程查询
```
```
./QueryData.out -h
,,--.                    .-,--.      .      
|`. | . . ,-. ,-. . .    ' |   \ ,-. |- ,-. 
|  .| | | |-' |   | |    , |   / ,-| |  ,-| 
`---\ `-^ `-' '   `-|    `-^--'  `-^ `' `-^ 
     `             /|                       
                  `-'                       

App description
Usage: ./QueryData.out [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n,--taskname TEXT REQUIRED task name (=main)
  -a,--address TEXT ...       host name (=127.0.0.1)
                               You can set up multi-IP addresses separated by spaces to take advantage of the server's multi-network card.
                               e.g. -a 192.168.0.2 192.168.0.3 192.168.0.4 192.168.0.5
  -p,--port INT               port number (=6327)
  -u,--user TEXT              user name (=sa)
  -w,--password TEXT          pass word (=admin)
  -s,--starttime TEXT         start time (=now)
                              format:
                               "YYYY-MM-DD hh:mm:ss.ms"
                               "mintime" start time is min UTC time : 1970-01-01 00:00:00.000
                               "now" start time is local real time
                               Enclosed in single or double quotes.
  -e,--endtime TEXT           end time (=forever)
                              format:
                               "YYYY-MM-DD hh:mm:ss.ms"
                               "forever" end time is max UTC time
  --search TEXT               search condition
  --first_point INT           first point's id (=1)
  --point_count INT           point count (=1)
  --point_interval INT        point interval (=1)
  --query_mode TEXT           query mode (=history_archived)
                               1.history_archived
                               2.history_archived_ex
                               3.plot_value
                               4.interval_value
                               5.summary_value
                               6.group_by_interval_summary_value
                               7.scan_snapshot_value
                               8.subscribe_snapshot_value
  --query_batch_count INT     query values count per batch (=1000)
  --interval INT              query interval (=1000)
  --thread_count INT          thread count (=1)
  --thread1_count INT         thread1 count (=1) section_summary_value use
  --print_log                 print log to console
  --log_level INT             log level (=2) as info
                               0.trace
                               1.debug
                               2.info
                               3.warn
                               4.err
                               5.critical
                               6.off
  --result_file TEXT          result file path, default is empty.
  --Attention                 Attention:
                              1.Time range with milliseconds can be passed in.
                              2.If multiple points are queried, they are automatically assigned to multiple query threads.
```

# 测试
在shell脚本配置参数：
```
#!/bin/sh
#后台执行命令
nohup ./QueryData.out -n test-1 -a 192.168.152.130 -s '2019-09-18 0:0:0.000' -e '2019-09-18 0:59:59.999' --first_point 121 --point_count 1 --result_file '/home/PerfTest/result/log.csv' --query_mode history_archived_ex --query_batch_count 18000 --thread_count 1 --log_level 5 >/dev/null 2>&1 &

nohup ./QueryData.out -n test-2 -a 192.168.152.130 -s '2019-09-18 0:0:0.000' -e '2019-09-18 0:59:59.999' --first_point 122 --point_count 1 --result_file '/home/PerfTest/result/log.csv' --query_mode history_archived_ex --query_batch_count 18000 --thread_count 1 --log_level 5 >/dev/null 2>&1 &

nohup ./QueryData.out -n test-3 -a 192.168.152.130 -s '2019-09-18 0:0:0.000' -e '2019-09-18 0:59:59.999' --first_point 123 --point_count 1 --result_file '/home/PerfTest/result/log.csv' --query_mode history_archived_ex --query_batch_count 18000 --thread_count 1 --log_level 5 >/dev/null 2>&1 &

nohup ./QueryData.out -n test-4 -a 192.168.152.130 -s '2019-09-18 0:0:0.000' -e '2019-09-18 0:59:59.999' --first_point 124 --point_count 1 --result_file '/home/PerfTest/result/log.csv' --query_mode history_archived_ex --query_batch_count 18000 --thread_count 1 --log_level 5 >/dev/null 2>&1 &

nohup ./QueryData.out -n test-5 -a 192.168.152.130 -s '2019-09-18 0:0:0.000' -e '2019-09-18 0:59:59.999' --first_point 125 --point_count 1 --result_file '/home/PerfTest/result/log.csv' --query_mode history_archived_ex --query_batch_count 18000 --thread_count 1 --log_level 5 >/dev/null 2>&1 &

nohup ./QueryData.out -n test-6 -a 192.168.152.130 -s '2019-09-18 0:0:0.000' -e '2019-09-18 0:59:59.999' --first_point 126 --point_count 1 --result_file '/home/PerfTest/result/log.csv' --query_mode history_archived_ex --query_batch_count 18000 --thread_count 1 --log_level 5 >/dev/null 2>&1
```
在命令行直接传入参数：
```
查询历史存储值：
./QueryData.out -n test -a 192.168.152.132 -s '2019-09-18 0:0:0.000' -e '2019-09-18 1:59:59.999' --first_point 121 --point_count 2 --query_mode history_archived_ex --query_batch_count 18000 --thread_count 1 --log_level 0

订阅快照值：
.\QueryData.exe -n test -u sa -w golden -a 192.168.152.176 -s "2019-01-1 0:0:0" -e "2020-12-31 23:59:59" --first_point 33663 --point_count 884 --query_mode subscribe_snapshot_value --thread_count 2 --log_level 0
```
