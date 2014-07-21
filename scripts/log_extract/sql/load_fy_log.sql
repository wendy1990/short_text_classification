--从本地载入日志记录表
USE fy_log;
LOAD DATA LOCAL INPATH "${LOG_PATH}/task_allocation.log" OVERWRITE 
    INTO TABLE task_allocation
    PARTITION (dt="${DATE}");
LOAD DATA LOCAL INPATH "${LOG_PATH}/task_collect.log" OVERWRITE 
    INTO TABLE task_collect
    PARTITION (dt="${DATE}");
LOAD DATA LOCAL INPATH "${LOG_PATH}/allocation_task_url_map.log" OVERWRITE 
    INTO TABLE allocation_task_url_map
    PARTITION (dt="${DATE}");
LOAD DATA LOCAL INPATH "${LOG_PATH}/collect_task_url_map.log" OVERWRITE 
    INTO TABLE collect_task_url_map
    PARTITION (dt="${DATE}");
LOAD DATA LOCAL INPATH "${LOG_PATH}/spider_status.log" OVERWRITE 
    INTO TABLE spider_status
    PARTITION (dt="${DATE}");
