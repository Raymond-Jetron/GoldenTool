#简介
GoldenTool为庚顿跨平台工具包，具体工具包含：

1. HistoryDataSync历史数据同步，适用于V3.0系列之间的同步，没有点数限制，没有授权限制。
2. MakeData数据生成工具，向数据库写入实时/历史数据，支持各种波形。
3. QueryData数据查询工具，从数据库查询数据，查快照、查历史存储值、查历史插值、查统计值、查趋势值。

#入门
1.	HistoryDataSync
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

> 详细内容见：/README/HistoryDataSync/README.md

2.	MakeData

```
.\MakeData.exe -h
,-,-,-.       .         .-,--.      .
`,| | |   ,-. | , ,-.   ' |   \ ,-. |- ,-.
  | ; | . ,-| |<  |-'   , |   / ,-| |  ,-|
  '   `-' `-^ ' ` `-'   `-^--'  `-^ `' `-^



App description
Usage: .\MakeData.exe [OPTIONS]

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
                               "YYYY-MM-DD hh:mm:ss"
                               "maxtime" start time is read snapshot max datetime
                               "now" start time is local real time
                               Enclosed in single or double quotes.
  -e,--endtime TEXT           end time (=forever)
                              format:
                               "YYYY-MM-DD hh:mm:ss"
                               "forever" end time is max UTC time
  -E,--elapsetime INT         elpase time (=1000) ms
  -i,--increment INT          increment time (=1000) ms
  -H,--history                put history data
  --low INT                   min value (=-100)
  --high INT                  max value (=100)
  -g,--generator TEXT         data generator (=sin), sin, line, rand, file
  --search TEXT               search condition
  --first_point INT           first point's id (=1)
  --point_count INT           point count (=1)
  --point_interval INT        point interval (=1)
  --write_mode TEXT           write mode (=time)
                               time : same time once
                               point : same point once
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
  --func_period INT           generator function period (=36)
                                when write_mode=point, write func_period*100 values count per batch, so set 3600 or more is better.
  --result_file TEXT          result file path, default is empty.
  --Attention                 # 注意：
                              #   1.当write_mode = point时，开始时间到结束时间的跨度不宜太长，取决于时间跨度内的文件数量，建议不要超过100个，可以多写几个命令，顺序执行
                              #   2.当write_mode = point时，增加参数--func_period 3600，每个点每批写入func_period * 100个，也就是同一个波形重复100次，调整这个参数可以控制每批写入的数据量，这里占用内存很少
                              #   3.当write_mode = time时，参数--func_period默认为36，会申请 2 * 8 * point_count*func_period 的内存，如果点数过多的话，会占用大量内存，故不宜设置太大，具体占多少内存合 适，可以用top命令查看
```

> 详细内容见：/README/MakeData/README.md

3. QueryData

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
                               6.section_summary_value
                               7.snapshot_value
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
  --Attention                 # 注意：
                              #   1.可以传入带毫秒的时间范围
                              #   2.如果查询多个标签点，会自动分配到多个线程查询
```

> 详细内容见：/README/QueryData/README.md

#生成与测试

1. Windows平台可用VS2017打开RTDBToolW.sln，或者用QT5.9打开RTDBTool.pro

2. Linux平台可用VS2017打开RTDBToolL.sln，或者用QT5.9打开RTDBTool.pro

