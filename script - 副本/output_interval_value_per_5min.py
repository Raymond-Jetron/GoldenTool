#功能说明：批量写入实时数据

import os
import subprocess


#当前路径
CUR_PATH=os.path.dirname(__file__)
#执行文件
exec_file=MakeDataL.exe
#执行程序路径
exe_path=os.path.join(CUR_PATH,exec_file)
print(exe_path)
#数据库IP
ip="192.168.152.129"
#数据库端口
port="6327"
#用户名
user="sa"
#密码
pwd="admin"
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
#起始点ID
first_point=121
#线程数
thread_count=1
#日志级别
log_level=2
#是否在控制台打印日志
print_log=1

args=exe_path+" --ip "+ip+" --start \""+start_time.__str__()+"\" --end \""+end_time.__str__()+"\" --interval "+str(time_interval)+" --thread_count "+str(thread_count)+" --points_dir \""+points_dir+"\" --source_from "+str(source_from)+" --interpolation_mode "+str(interpolation_mode)+" --log_level "+str(log_level)+" --print_log "+str(print_log)+" --output_target "+str(output_target)+" --aggregation "+str(aggregation)+" --output_title "+str(output_title)+" --topic "+topic+" --broker_list "+broker_list+" --partition "+str(partition)+" --message_count "+str(message_count)+" --after_cmd \"\""
print(args)
child = subprocess.Popen(args)
child.wait()
print("本次执行完毕")
print ('----------')


