#!/bin/bash
#该脚本根据FengYi log提取结果，统计封姨每天的抓取状态，具体see sql
date=$1
loc_tmp_dir=/home/app/windbell/fengyi/tmp4log
HIVE_HOME=/home/app/srv/hive
export PATH=$HIVE_HOME/bin:$PATH
rm tmp/statistic.sql
#sed "s/{yyyy-mm-dd}/$date/g" sql/statistic_single.template > tmp/statistic.sql
#hive -S -f tmp/statistic.sql > $loc_tmp_dir/overall_result

#整体数据概览
sed "s/{yyyy-mm-dd}/$date/g" sql/statistic_union.template | sed "s#{LOC_TMP_DIR}#$loc_tmp_dir/overall_res#g" > tmp/statistic.sql
hive -f tmp/statistic.sql

#brief: 构造spider统计脚本并执行
#$1: [schedule_asc|schedule_desc|page_asc|page_desc] $2: order by $3: [asc|desc] $4: limit_count
querySpd(){
    rm tmp/spd_sta.sql
    sed "s/{yyyy-mm-dd}/$date/g" sql/spd_sta.template |
    sed "s#{LOC_TMP_DIR}#$loc_tmp_dir/spider_res/$1#g" |
    sed "s/{ORDER_FIELD}/$2/g" |
    sed "s/{ORDER_TYPE}/$3/g" > tmp/spd_sta.sql
    if [ "$4" = "ALL" ]; then
        sed -i "s/{LIMIT_COUNT}//g" tmp/spd_sta.sql
    else
        sed -i "s/{LIMIT_COUNT}/limit $4/g" tmp/spd_sta.sql
    fi
    hive -f tmp/spd_sta.sql
}

#统计爬虫效率
#按照领取任务数升序排列
#`querySpd schedule_asc schedule_task asc 10`
#按照领取任务数降序排列
#`querySpd schedule_desc schedule_task desc 10`
#按照上报page数升序排列
#`querySpd page_asc schedule_task asc 10`
#按照上报page数降序排列
#`querySpd page_desc schedule_task desc 10`
#按照id升序排列
`querySpd id_asc ss.spider_id asc ALL`


#brief: 向*_row.template中填入一行数据
#$1: template file, $2 template name, $3 value list

insertData()
{
    row_data=`cat $1`
    idx=1
    for value in `echo "$3" | sed 's/|/ /g'`
    do
        row_data=$(echo "$row_data" | sed "s/{$2$idx}/$value/g")
        idx=`expr $idx + 1`
    done
    echo "$row_data"
}

#tmp_mail=`cat html/mail.template`
tmp_mail=`cat html/mail_allspider.template`
rm tmp/mail.html
#brief: 向spider_row.template中填入多行数据
#$1: type[schedule_desc|schedule_asc|page_desc|page_asc] $2: template name
insertSpiderData(){
    spider_resultlines=`sed 's/\x01/|/g' "$loc_tmp_dir/spider_res/$1/000000_0"`
    idx=1
    for spider_resultline in $spider_resultlines
    do
        spider_row=`insertData html/spider_row.template SPIDER_VALUE $spider_resultline`
        tmp_mail=`echo "$tmp_mail" | sed -e "s|{$2}|$spider_row{$2}|g" -e "s|{INDEX}|$idx|g"`
        idx=`expr $idx + 1`
    done
    echo "$tmp_mail" | sed "s|{$2}||g"
}

#生成Overall数据行
overall_resultline=`awk -F '_' '{print $2}' "$loc_tmp_dir/overall_res/000000_0"`
overall_resultline=`echo $overall_resultline | sed 's/ /|/g'`
overall_row=`insertData html/overall_row.template OVERALL_VALUE $overall_resultline`
#填充Overall数据行
tmp_mail=`echo "$tmp_mail" | sed "s|{OVERALL_ROW}|$overall_row|g"`

#填充Spider数据行
#schedule_asc
#tmp_mail=`insertSpiderData schedule_asc SPIDER_ROW_SCHEDULE_ASC`
#schedule_desc
#tmp_mail=`insertSpiderData schedule_desc SPIDER_ROW_SCHEDULE_DESC`
#page_asc
#tmp_mail=`insertSpiderData page_asc SPIDER_ROW_PAGE_ASC`
#page_desc
#tmp_mail=`insertSpiderData page_desc SPIDER_ROW_PAGE_DESC`
#id_asc
tmp_mail=`insertSpiderData id_asc SPIDER_ROW_ID_ASC`

echo "$tmp_mail" > tmp/mail.html

#发送邮件
#python python/send_fy_log_mail.py 'fm_monitor@fmkj.net' 'vincent.huang@fmkj.net; zuo.jin@fmkj.net; elvin.xu@fmkj.net; tim.yao@fmkj.net; homer.he@fmkj.net; henry.he@fmkj.net; tony.liu@fmkj.net; blond.li@fmkj.net; dean.wu@fmkj.net; ronnie.zhang@fmkj.net; max.zhang@fmkj.net' "FengYi log daily report [$date]" tmp/mail.html
python python/send_fy_log_mail.py 'fm_monitor@fmkj.net' 'max.zhang@fmkj.net' "FengYi log daily report [$date]" tmp/mail.html

exit 0
