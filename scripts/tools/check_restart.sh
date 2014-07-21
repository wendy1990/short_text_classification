#!/bin/bash
#检查Bigben系统中PrepareServer,TextClassificationServer和PageMarkServer是否存在，如果不存在，重启
#Usage: nohup sh check_restart.sh [prepare_svr|prepare_proxy|classification_svr|pagemark_svr] &

_proc=$1
bigben_root=/home/app/windbell/bigben
host=`hostname`
date=`date +%F`
start_classification_svr()
{
        cd $bigben_root/
        mv core "core-`date +%s`"
        nohup bin/classification_svr -f conf/server_conf.config > $bigben_root/logs/console.classification_svr 2>&1 &
        python $bigben_root/scripts/tools/send_mail.py 'fm_monitor@fmkj.net' 'raynard.pan@fmkj.net,eric.zhang@fmkj.net,zuo.jin@fmkj.net' 'ClassificationServer Warning' "[$date] [$host] $_proc has be restart at `date +%c`!"
}
start_prepare_svr()
{
        cd $bigben_root/
        mv core "core-`date +%s`"
        nohup bin/prepare_svr -f conf/prepare.conf > $bigben_root/logs/console.prepare_svr 2>&1 &
        python $bigben_root/scripts/tools/send_mail.py 'fm_monitor@fmkj.net' 'eric.zhang@fmkj.net,zuo.jin@fmkj.net' 'PrepareServer Warning' "[$date] [$host] $_proc has be restart at `date +%c`!"
}
start_prepare_proxy()
{
        cd $bigben_root/
        mv core "core-`date +%s`"
        nohup bin/prepare_proxy -f conf/prepare_proxy.conf >$bigben_root/logs/console.prepare_parxy 2>&1 &
        python $bigben_root/scripts/tools/send_mail.py 'fm_monitor@fmkj.net' 'eric.zhang@fmkj.net,zuo.jin@fmkj.net' 'PrepareProxyServer Warning' "[$date] [$host] $_proc has be restart at `date +%c`!"
}
start_pagemark_svr()
{
        cd $bigben_root/
        nohup java -jar bin/PageMarkServer-1.0.jar conf/page-mark.properties conf/page-mark.server >$bigben_root/logs/console.page_mark 2>&1 &
        python $bigben_root/scripts/tools/send_mail.py 'fm_monitor@fmkj.net' 'max.zhang@fmkj.net,eric.zhang@fmkj.net,zuo.jin@fmkj.net' 'PageMarkServer Warning' "[$date] [$host] $_proc has be restart!"
}

while true
do
        num=`ps --no-heading -C $_proc|wc -l`
        #num=`ps xuf|grep "$_proc"|grep -v 'grep'|grep -v 'check_restart'|wc -l`
        #echo $num
        #echo `ps xuf|grep $_proc`
        case $_proc in
                "classification_svr")
                if [ "0" -eq "$num" ]; then
                        start_classification_svr
                fi
                ;;
                "prepare_svr")
                if [ "0" -eq "$num" ]; then
                        start_prepare_svr
                fi
                ;;
                "prepare_proxy")
                if [ "0" -eq "$num" ]; then
                        start_prepare_proxy
                fi
                ;;
                "PageMarkServer")
                num=`ps xuf|grep "PageMarkServer"|grep -v 'grep'|grep -v 'check_restart'|wc -l`
                if [ "0" -eq "$num" ]; then
                        start_pagemark_svr
                fi
                ;;
               *)
                echo "unknowed proc name "$_proc
                break
                ;;
        esac

        sleep 1s
done
