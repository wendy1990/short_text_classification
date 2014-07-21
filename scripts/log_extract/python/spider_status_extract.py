#!/usr/bin/env python
#-*-coding=utf8-*-
"""
module: spider
usage: python spider_status_extract.py PATHTO-spider_status output-path
    PATHTO-spider_status: spider_status.txt文件路径
    output-path: 结果输出目录
brief: 完成当前服务器上spider_status日志的抽取
author: zyy_max
date: 2013-12-09

"""
import time
import sys
import os

def stamp2time(stamp, format_type='%Y-%m-%d %H:%M:%S'):
    return time.strftime(format_type, time.localtime(stamp))
if __name__=="__main__":
    if  len(sys.argv) < 2:
        print "Usage:\tsh spider_status_extract.sh PATHTO-spider_status output-path"
        print "e.g.\tPATHTO-spider_status: /home/elvin.xu/fengyi/release/test/spider_status.txt"
        print "\toutput-path: /home/max.zhang/tmp/local/2013-12-09"
        exit(-1)

    spider_status_path, output_path = sys.argv[1:]
    hostname = os.popen('hostname').read().strip()
    result_lines = []
    with open(spider_status_path) as ins:
        for line in ins:
            data_list = line.strip().split()
            data_list[4] = stamp2time(float(data_list[4]))
            data_list[5] = stamp2time(float(data_list[5]))
            result_lines.append('\t'.join(data_list)+'\n')
    with open(os.path.join(output_path,'spider_status_'+hostname), 'w') as outs:
        outs.writelines(result_lines)





