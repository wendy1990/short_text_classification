#!/bin/bash
#该脚本统计日志文件中最近一小时内LOG4STATISTIC的个数
log_file=$1
last_hour=`date "+%F %H" -d '-1 hour'`
process_count=`cat $log_file |
grep 'LOG4STATISTIC' |
grep "$last_hour" |
wc -l`
echo "`date '+%F %H' -d '-1 hour'`,$process_count"
