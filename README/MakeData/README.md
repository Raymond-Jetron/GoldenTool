#简介
MakeData，数据生成工具。
.exe的为windows版，.out的为linux版。

#入门
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

```
./MakeDataL.out -h
,-,-,-.       .         .-,--.      .      
`,| | |   ,-. | , ,-.   ' |   \ ,-. |- ,-. 
  | ; | . ,-| |<  |-'   , |   / ,-| |  ,-| 
  '   `-' `-^ ' ` `-'   `-^--'  `-^ `' `-^ 
                                           
                                           

App description
Usage: ./GoldenMakeDataL.out [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n,--taskname TEXT REQUIRED task name (=main)
  -a,--address TEXT ...       host name (=127.0.0.1)
                               You can set up multi-IP addresses separated by spaces to take advantage of the server's multi-network card.
                               e.g. -a 192.168.0.2 192.168.0.3 192.168.0.4 192.168.0.5
  -p,--port INT               port number (=6327)
  -u,--user TEXT              user name (=sa)
  -w,--password TEXT          pass word (=golden)
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
  --Attention                 Attention:
                              1.When write_mode = point, The start-to-end time span should not be too long,
                               depending on the number of archive files in the time span.
                               It is not recommended to exceed 100.
                               You can write a few more commands and execute them sequentially.
                              2.When write_mode = point, Add the parameter --func_period 3600.
                               Write func_period * 100 values per batch per point,
                               i.e. 100 repetitions of the same waveform.
                               Adjusting this parameter controls the amount of data written per batch,
                               and it uses very little memory.
                              3.When write_mode = time, Add the parameter --func_period 36.
                               2 * 8 * point_count * func_period Byte memory will be requested.
                               If you have too many points, it will take up a lot of memory,
                               so it is not appropriate to set too much.
                               You can view it using the top command.
```

#测试
在shell脚本配置参数：
```
#!/bin/sh
#
# 按标签点为单位批量写大时间范围的快照数据
# 注意：
#   1.开始时间到结束时间的跨度不宜太长，取决于时间跨度内的文件数量，建议不要超过100个，可以多写几个命令，顺序执行
#   2.当write_mode=point时，增加参数--func_period 3600，每个点每批写入func_period*100个，也就是同一个波形重复100次，调整这个参数可以控制每批写入的数据量，这里占用内存很少
#   3.当write_mode=time时，参数--func_period默认为36，会申请 2*8*point_count*func_period 的内存，如果点数过多的话，会占用大量内存，故不宜设置太大，具体占多少内存合适，top命令自己看着调吧
./MakeData.out -n test_01 -a 192.168.70.233 --write_mode point -s '2019-09-06 23:00:00' -e '2019-09-08 23:59:59' --log_level 3 -E 0 -i 200 --first_point 1 --point_count 960000 --thread_count 48 --func_period 3600
./MakeData.out -n test_02 -a 192.168.70.233 --write_mode point -s '2019-09-09 00:00:00' -e '2019-09-11 23:59:59' --log_level 3 -E 0 -i 200 --first_point 1 --point_count 960000 --thread_count 48 --func_period 3600
./MakeData.out -n test_03 -a 192.168.70.233 --write_mode point -s '2019-09-12 00:00:00' -e '2019-09-14 23:59:59' --log_level 3 -E 0 -i 200 --first_point 1 --point_count 960000 --thread_count 48 --func_period 3600
./MakeData.out -n test_04 -a 192.168.70.233 --write_mode point -s '2019-09-15 00:00:00' -e '2019-09-17 23:59:59' --log_level 3 -E 0 -i 200 --first_point 1 --point_count 960000 --thread_count 48 --func_period 3600
./MakeData.out -n test_05 -a 192.168.70.233 --write_mode point -s '2019-09-18 00:00:00' -e '2019-09-20 23:59:59' --log_level 3 -E 0 -i 200 --first_point 1 --point_count 960000 --thread_count 48 --func_period 3600
```
在命令行直接传入参数：
```
./MakeData.out -n test_01 -a 192.168.70.233 --write_mode point -s '2019-09-06 23:00:00' -e '2019-09-08 23:59:59' --log_level 3 -E 0 -i 200 --first_point 1 --point_count 960000 --thread_count 48 --func_period 3600
```
