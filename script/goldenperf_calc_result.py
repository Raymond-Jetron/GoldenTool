#!/usr/bin/python
# -*- coding:utf-8 -*-
''' 
使用说明：
[参数1] 生成文件的程序名，有两个可选： GoldenMakeData   GoldenQueryData
传入相对路径：
python goldenperf_calc_result.py GoldenMakeData result/log.csv
传入绝对路径：
python goldenperf_calc_result.py GoldenQueryData /home/golden/projects/GoldenPerf/result/log.csv
 '''
import sys
import os
import time

def main(argv):
    exec_name = argv[1]
    result_file_path = argv[2]
    result_file_path = os.path.abspath(result_file_path)
    print("result file path : {}".format(result_file_path))
    print("The test results are listed below:")
    while os.path.exists(result_file_path)==False :
        time.sleep(1)
        print("file not exists, sleep 1s")
    with open(result_file_path,"r") as f:
        if exec_name == "GoldenQueryData" :
            avg_parallel_thread_time = 0.
            avg_all_thread_all_call_total_elapsed = 0.
            all_user_query_total_count = 0
            users_count = 0
            for row in f.readlines():
                row = row.strip('\n')
                fields = row.split(',')
                task_name = fields[0]
                parallel_thread_time = float(fields[1])
                all_thread_all_call_total_elapsed = float(fields[2])
                query_total_count = int(fields[3])
                print("tast = {}, parallel_thread_time = {:.2f}ms, all_thread_all_call_total_elapsed = {:.2f}ms, query_total_count = {}".format(task_name,parallel_thread_time,all_thread_all_call_total_elapsed,query_total_count))
                avg_parallel_thread_time += parallel_thread_time
                avg_all_thread_all_call_total_elapsed += all_thread_all_call_total_elapsed
                all_user_query_total_count += query_total_count
                users_count += 1
            print("===================================================")
            print("avg_parallel_thread_time = {:.2f}ms".format(avg_parallel_thread_time / users_count))
            print("sum_all_thread_all_call_total_elapsed = {:.2f}ms".format(avg_all_thread_all_call_total_elapsed))
            print("all_user_query_total_count = {}".format(all_user_query_total_count))
            print("===================================================")
        elif exec_name == "GoldenMakeData" :
            avg_put_one_point_full_data_elapsed = 0.
            avg_put_one_time_full_data_elapsed = 0.
            all_user_make_total_count = 0
            users_count = 0
            for row in f.readlines():
                row = row.strip('\n')
                fields = row.split(',')
                task_name = fields[0]
                put_one_point_full_data_elapsed = float(fields[1])
                put_one_time_full_data_elapsed = float(fields[2])
                make_total_count = int(fields[3])
                print("task = {}, put_one_point_full_data_elapsed = {:.2f}ms, put_one_time_full_data_elapsed = {:.2f}ms, make_total_count = {}".format(task_name,put_one_point_full_data_elapsed,put_one_time_full_data_elapsed,make_total_count))
                avg_put_one_point_full_data_elapsed += put_one_point_full_data_elapsed
                avg_put_one_time_full_data_elapsed += put_one_time_full_data_elapsed
                all_user_make_total_count += make_total_count
                users_count += 1
            print("===================================================")
            print("avg_put_one_point_full_data_elapsed = {:.2f}ms".format(avg_put_one_point_full_data_elapsed / users_count))
            print("avg_put_one_time_full_data_elapsed = {:.2f}ms".format(avg_put_one_time_full_data_elapsed / users_count))
            print("all_user_make_total_count = {}".format(all_user_make_total_count))
            print("===================================================")
        else :
            print("Invalid parameter, you must choose from : [GoldenMakeData GoldenQueryData]")

if __name__ == '__main__':
    main(sys.argv)
