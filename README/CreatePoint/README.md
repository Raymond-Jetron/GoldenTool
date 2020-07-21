#简介
CreatePoint，批量建点工具。
.exe的为windows版，.out的为linux版。

#入门
```
.\CreatePoint.exe -h
 ,--.             .       .-,--.           .
| `-' ,-. ,-. ,-. |- ,-.   '|__/ ,-. . ,-. |-
|   . |   |-' ,-| |  |-'   ,|    | | | | | |
`--'  '   `-' `-^ `' `-'   `'    `-' ' ' ' `'



App description
Usage: D:\golden-data\GoldenTool\publish\CreatePoint-1.0.8-win-x86_64\QueryData\x86\CreatePoint.exe [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n,--taskname TEXT REQUIRED task name (=main)
  -a,--address TEXT ...       host name (=127.0.0.1)
                               You can set up multi-IP addresses separated by spaces to take advantage of the server's multi-network card.
                               e.g. -a 192.168.0.2 192.168.0.3 192.168.0.4 192.168.0.5
  -p,--port INT               port number (=6327)
  -u,--user TEXT              user name (=sa)
  -w,--password TEXT          pass word (=admin)
  --thread_count INT          thread count (=1)
  --points_dir TEXT REQUIRED  point file directory (*.csv)
  --print_log                 print log to console
  --log_level INT             log level (=2) as info
                               0.trace
                               1.debug
                               2.info
                               3.warn
                               4.err
                               5.critical
                               6.off
  --ecoding TEXT              Encoding character sets (=gb2312)
                               "gb2312" or "utf-8"
  --result_file TEXT          result file path, default is empty.
  --Attention                 Attention:
                              1.Time range with milliseconds can be passed in.
                              2.If multiple points are queried, they are automatically assigned to multiple query threads.
```
