#!/bin/sh
#Author zyy_max
#Date   2013-12-06
#Brief  完成当前服务器上Fengyi系统的日志抽取和上传
#Usage  sh local_report.sh date dfs_log_path
if [ $# -lt 2 ]; then
    echo "Usage sh local_report.sh date dfs_log_path"
    exit 0
fi
#进入日志抽取脚本所在目录
#cd /home/wisha.he/log_statistisc 
cd /home/max.zhang/git/bigben/scripts/log_extract/python
#设置本地临时目录
tmp_path=/home/max.zhang/tmp/local
if [ ! -d $tmp_path ];then
    mkdir -p $tmp_path
    chmod 777 $tmp_path
fi
#执行日志抽取脚本
#$1:date(2013-12-06) 
python log_extract.py $1 "/home/elvin.xu/fengyi" $tmp_path "ALL"
#抽取spider_status
python spider_status_extract.py "/home/elvin.xu/fengyi/release/test/spider_status.txt" $tmp_path
#将临时目录中的抽取结果put到HDFS上
for file in `ls $tmp_path`
do
    if [ -f "$tmp_path/$file" ];then
        #$2:dfs_log_path
        #在ssh中调用时只能使用全路径？
        /home/app/srv/hadoop/bin/hadoop fs -put $tmp_path/$file $2
    fi
done
    
