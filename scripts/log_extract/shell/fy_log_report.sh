#!/bin/sh
#Author zyy_max
#Date   2013-12-06
#Brief  生成Fengyi系统的日志报表，包含任务分派(schedule)，任务回收(collect)，spider在线情况，以及分派和回收任务下task与url的对应表
#Usage  sh fy_log_report.sh 2013-12-06

if [ $# -lt 1 ]; then
    echo "usage: sh fy_log_report.sh 2013-12-06"
    exit 0
fi

#当前日期
date=$1
#当前日期的log记录表在HDFS中的路径
dfs_log_path="/test/max.zhang/log_statistics/Fengyi/$date"

#脚本文件路径
shell_path="/home/max.zhang/git/bigben/scripts/log_extract/shell"
sql_path="/home/max.zhang/git/bigben/scripts/log_extract/sql"

#创建当前日期的log目录
hadoop fs -rmr $dfs_log_path
hadoop fs -mkdir $dfs_log_path
hadoop fs -chmod a+w $dfs_log_path
#执行远端主机上的日志抽取脚本
ssh 10.1.160.4 "sh $shell_path/local_report.sh $date $dfs_log_path"

#将HDFS中的日志统计结果下载到本地
tmp_root="/home/max.zhang/tmp/log4hive"
if [ ! -d $tmp_root ];then
    mkdir -p $tmp_root
    chmod 777 $tmp_root
fi
if [ -d $tmp_root/$date ];then
    rm -rf $tmp_root/$date
fi
hadoop fs -get $dfs_log_path $tmp_root
#合并多个log记录文件
echo $tmp_root
cat $tmp_root/$date/task_allocation_* > "$tmp_root/task_allocation.log"
cat $tmp_root/$date/task_collect_* > "$tmp_root/task_collect.log"
cat $tmp_root/$date/allocation_task_url_map_* > "$tmp_root/allocation_task_url_map.log"
cat $tmp_root/$date/collect_task_url_map_* > "$tmp_root/collect_task_url_map.log"
cat $tmp_root/$date/spider_status_* > "$tmp_root/spider_status.log"
#创建临时目录，以放置待发送的邮件内容
if [ ! -d $tmp_root/mail/$date ];then
    mkdir -p $tmp_root/mail/$date
    chmod 777 $tmp_root
fi
#将log数据载入Hive中
hive -d LOG_PATH=$tmp_root -d DATE=$date -f $sql_path/load_fy_log.sql
#message=`cat tmp/$date/query_result`
#echo $message
#发送邮件

#python send_mail.py from to subject $message

