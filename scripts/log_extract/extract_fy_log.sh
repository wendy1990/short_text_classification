#!/bin/bash
#该脚本完成FengYi日志的抽取
#usage: sh extract_fy_log.sh 2013-12-01

date=$1
hadoop_root='/windbell/fengyi/log_statistic/'
loc_tmp_dir='/home/app/windbell/fengyi/tmp4log/'
collect_svr_list="H8.CLO.7FB15.NJDX.FMKJ.NET H9.CLO.7FB15.NJDX.FMKJ.NET"
schedule_svr_list="H6.CLO.7FB15.NJDX.FMKJ.NET H7.CLO.7FB15.NJDX.FMKJ.NET"
svr_root="/home/app/windbell/fengyi/"
hadoop_bin="/home/app/srv/hadoop/bin"
HADOOP_HOME="/home/app/srv/hadoop"
HIVE_HOME="/home/app/srv/hive"
export PATH=$HADOOP_HOME/bin:$HIVE_HOME/bin:$PATH

idx=0
for i in $collect_svr_list
do
	ssh $i "mkdir -p /home/app/bigben/scripts"
	scp -r /home/app/bigben/scripts/log_extract $i:/home/app/bigben/scripts/
	#make local tmp dir
	ssh $i "mkdir -p $loc_tmp_dir"
	#run extract script
	nohup ssh $i "python /home/app/bigben/scripts/log_extract/python/log_extract.py $date $svr_root $loc_tmp_dir task_collect" >/dev/null 2>&1 &
	nohup ssh $i "python /home/app/bigben/scripts/log_extract/python/log_extract.py $date $svr_root $loc_tmp_dir collect_task_url_map" >/dev/null 2>&1 &
	idx=`expr $idx + 2`
done

for i in $schedule_svr_list
do
	ssh $i "mkdir -p /home/app/bigben/scripts"
	scp -r /home/app/bigben/scripts/log_extract $i:/home/app/bigben/scripts/
	ssh $i "mkdir -p $loc_tmp_dir"
	nohup ssh $i "python /home/app/bigben/scripts/log_extract/python/log_extract.py $date $svr_root $loc_tmp_dir task_allocation" >/dev/null 2>&1 &
	nohup ssh $i "python /home/app/bigben/scripts/log_extract/python/log_extract.py $date $svr_root $loc_tmp_dir allocation_task_url_map" >/dev/null 2>&1 &
	idx=`expr $idx + 2`
done

#get spider status
mkdir -p $loc_tmp_dir
ssh H6.CLO.7FB15.NJDX.FMKJ.NET "cd /home/app/windbell/fengyi/test; sh conf_status.sh" > ${loc_tmp_dir}spider_status.txt
sed -i "s/ /\t/g" ${loc_tmp_dir}spider_status.txt

#add partitions in Hive
#hive -e "use fy_log; alter table spider_status add partition (dt='$date');
#         alter table collect_task_url_map add partition (dt='$date');
#         alter table task_collect add partition (dt='$date');
#         alter table allocation_task_url_map add partition (dt='$date');
#         alter table task_allocation add partition (dt='$date');"
hive -e "use fy_log; alter table spider_status add partition (dt='$date');"
hive -e "use fy_log; alter table collect_task_url_map add partition (dt='$date');"
hive -e "use fy_log; alter table task_collect add partition (dt='$date');"
hive -e "use fy_log; alter table allocation_task_url_map add partition (dt='$date');"
hive -e "use fy_log; alter table task_allocation add partition (dt='$date');"

#put extract result to HDFS
hadoop fs -rmr ${hadoop_root}spider_status/dt=$date
hadoop fs -mkdir ${hadoop_root}spider_status/dt=$date
hadoop fs -put ${loc_tmp_dir}spider_status.txt ${hadoop_root}spider_status/dt=$date

#wait child process
for i in `seq 1 $idx`
do
	wait
done

idx=0
hadoop fs -rmr ${hadoop_root}collected_task-url_map/dt=$date
hadoop fs -rmr ${hadoop_root}task_collected/dt=$date
hadoop fs -mkdir ${hadoop_root}collected_task-url_map/dt=$date
hadoop fs -mkdir ${hadoop_root}task_collected/dt=$date
for i in $collect_svr_list
do
	nohup ssh $i "source ~/.bashrc; $hadoop_bin/hadoop fs -put $loc_tmp_dir/collect_task_url_map_* ${hadoop_root}collected_task-url_map/dt=$date" >/home/app/windbell/fengyi/tmp4log/put_ctum.log 2>&1 &
	nohup ssh $i "source ~/.bashrc; $hadoop_bin/hadoop fs -put $loc_tmp_dir/task_collect_* ${hadoop_root}task_collected/dt=$date" >/dev/null 2>&1 &
	idx=`expr $idx + 2`
done
hadoop fs -rmr ${hadoop_root}allocation_task-url_map/dt=$date
hadoop fs -rmr ${hadoop_root}task_allocation/dt=$date
hadoop fs -mkdir ${hadoop_root}allocation_task-url_map/dt=$date
hadoop fs -mkdir ${hadoop_root}task_allocation/dt=$date
for i in $schedule_svr_list
do
	nohup ssh $i "source ~/.bashrc; $hadoop_bin/hadoop fs -put $loc_tmp_dir/allocation_task_url_map_* ${hadoop_root}allocation_task-url_map/dt=$date" >/dev/null 2>&1 &
	nohup ssh $i "source ~/.bashrc; $hadoop_bin/hadoop fs -put $loc_tmp_dir/task_allocation_* ${hadoop_root}task_allocation/dt=$date" >/dev/null 2>&1 &
	idx=`expr $idx + 2`
done

for i in `seq 1 $idx`
do
	wait
done
