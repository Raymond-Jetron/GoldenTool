@ECHO OFF
REM 日期：2019-11-06
REM 作者：王晋晖
REM 功能说明：调用python脚本

cd %~dp0
python .\output_interval_value_per_5min.py
echo 执行完毕