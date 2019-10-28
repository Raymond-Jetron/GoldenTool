#!/bin/sh
#工作空间
work_space=/home/golden/projects/GoldenPerf/
#执行文件
exec_file=GoldenMakeDataL.out
#执行文件路径
exec_root_path=/home/golden/projects/GoldenPerf/GoldenMakeDataL/bin/x64/Release/
#输出结果文件
result_path=result/make_result.csv
#启动进程数
process_count=6
#任务名，后面用序号加后缀
task_name=mkdata_
#数据库IP地址
rtdb_address=192.168.152.130
#开始时间
start_time='2019-10-02 13:04:00'
#结束时间
end_time='2019-10-02 13:04:20'
#写入模式
write_mode=time
#日志级别
log_level=3
#执行周期，单位ms
elapsetime=200
#时间戳递增间隔，单位ms
increment=200
#第一个进程起始点ID
first_point=121
#每进程负责的标签点数量
point_count=1
#每进程的线程数
thread_count=1

#批量启动
for((i=0;i<process_count;i++))
do
echo "start "$task_name$i
nohup $exec_root_path$exec_file -n $task_name$i -a $rtdb_address --write_mode $write_mode -s "$start_time" -e "$end_time" -E $elapsetime -i $increment --thread_count $thread_count --point_count $point_count --first_point $(($i*$point_count+$first_point)) --result_file $work_space$result_path >/dev/null 2>&1 &
done

#监控后台进程
while :
do
  echo "monitor backgroud processes"
  echo "================================="
  jobs
  sleep 1s
  jobs_info=$(jobs)
  if [ ${#jobs_info} -lt 100 ]
  then
    echo "finished."
    break
  fi
  time=$(date "+%Y-%m-%d %H:%M:%S")
  echo "======="$time"======="
  sleep 1s
  clear
done

cd $work_space
python script/goldenperf_calc_result.py GoldenMakeData $result_path