use fy_log;
 select    concat_ws('_', '7', string(avg(unix_timestamp(tc.end_time)-unix_timestamp(tc.start_time)))) as overall
        from      task_collect tc
        join
            (
                  select task_seq from task_allocation where dt='2013-12-20'
            )tmp_ta
        on  (tmp_ta.task_seq=tc.task_seq)
        where     tc.dt='2013-12-20'
