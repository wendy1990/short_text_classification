#!/bin/bash
#该脚本完成FengYi日志的抽取、统计和报表
#usage: sh execute_fy_log.sh 2013-12-01

date=$1
sh extract_fy_log.sh $1 > /home/app/windbell/fengyi/tmp4log/extract.log 2>&1
sh statistic_fy_log.sh $1 > /home/app/windbell/fengyi/tmp4log/statistic.log 2>&1
