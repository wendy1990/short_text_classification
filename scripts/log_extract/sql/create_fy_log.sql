--Fengyi日志统计记录表

--创建数据库
CREATE DATABASE IF NOT EXISTS fy_log;

--创建数据表
USE fy_log;

--spider任务分派记录表
--task_seq: spider领取任务的序列号
--url_num: 该任务包含多少个URL
--spider_id：是那台spider领取的这个任务
--data_time: 该动作发生时间
--以日期作为partition
CREATE EXTERNAL TABLE IF NOT EXISTS task_allocation(
    task_seq STRING,
    url_num INT,
    spider_id INT,
    data_time STRING
)
PARTITIONED BY (dt STRING)
ROW FORMAT DELIMITED FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE
location '/windbell/fengyi/log_statistic/task_allocation';


--collect server任务回收记录表
--task_seq: spider领取任务的序列号
--page_num: 实际匹配到的（是目标范围内的）页面数
--spider_id：是那台spider领取的这个任务
--start_time: spider接收到该任务的时间
--end_time: spider完成这个任务的时间
--data_time: collect server收到该任务的时间
--以日期作为partition
CREATE EXTERNAL TABLE IF NOT EXISTS task_collect(
    spider_id INT,
    task_seq STRING,
    page_num INT,
    start_time STRING,
    end_time STRING,
    data_time STRING
)
PARTITIONED BY (dt STRING)
ROW FORMAT DELIMITED FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE
location '/windbell/fengyi/log_statistic/task_collected';


--spider在线情况表
--cur_IP: 当前的IP地址
--MAC：spider所在的物理设备硬件地址
--version:spider当前的版本号
--first_reg_time: 最初的登录时间
--last_login_tim: 最后的登录时间
--login_cnt: 登录次数
--以日期作为partition
CREATE EXTERNAL TABLE IF NOT EXISTS spider_status(
    spider_id INT,
    cur_IP STRING,
    MAC STRING,
    box_id STRING,
    location STRING 
)
PARTITIONED BY (dt STRING)
ROW FORMAT DELIMITED FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE
location '/windbell/fengyi/log_statistic/spider_status';


--任务分配时task与url对应表
--task_seq: spider领取任务的序列号
--url1－n：该任务包含的url
--以日期作为partition
CREATE EXTERNAL TABLE IF NOT EXISTS allocation_task_url_map(
    task_seq STRING,
    url STRING
)
PARTITIONED BY (dt STRING)
ROW FORMAT DELIMITED FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE
location '/windbell/fengyi/log_statistic/allocation_task-url_map';


--任务回收时task与url对应表
--task_seq: spider领取任务的序列号
--url1－n：实际上报的url列表，如果没有则为空
--以日期作为partition
CREATE EXTERNAL TABLE IF NOT EXISTS collect_task_url_map(
    task_seq STRING,
    url STRING
)
PARTITIONED BY (dt STRING) 
ROW FORMAT DELIMITED FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE
location '/windbell/fengyi/log_statistic/collected_task-url_map';
