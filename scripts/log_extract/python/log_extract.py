#!/usr/bin/env python
#coding=utf-8
"""
module: log_extract.py
usage: python log_extract.py date log-path output-path [table-name]
    date: 要提取那天的日志
    log-path: 日志文件所在路径
    output-paht: 结果输出目录
    table-name: 要从日志中提取生成那几个table. 目前有[task_allocation | task_collect | allocation_task_url_map | collect_task_url_map | ALL]
brief: 这个脚本提取FengYi各个角色的日志成结构化数据，为后面的hive分析提供数据基础
    输出结果文件文table-name_hostname
author: wisha.he@fmkj.net; angelian.feng@fmkj.net; max.zhang@fmkj.net; zuo.jin@fmkj.net
date: 2013-12-05

"""

import time
import sys
import os

def timematch(item,item_templet):
    return  item.split(']')[0].split('[')[1].split(' ')[0]==item_templet
     
def stamp2time(stamp, format_type='%Y-%m-%d %H:%M:%S'):
    return time.strftime(format_type, time.localtime(stamp))

#@brief parse the assign task item
#@param a line of schedule_log file 
def assign_task_item_parse(item):
    item_list = item.split(',')
    content = (item_list[0].strip().split('id')[1].strip() + '\t'
             + item_list[1].strip().split(' ')[2] + '\t'
             + item_list[2].strip().split('[')[0].strip().split(' ')[1] + '\t'
             + item_list[0].strip().split(']')[0].replace('[','') + '\n'
              )
#    print content
    return content

#@brief parse the spider status item
#@param a line of spider_status.txt line
def spider_status_item_parse(item):
    item_list = item.split(' ')
    content = (item_list[0].strip() +'\t' 
             + item_list[1].strip() + '\t'
             + item_list[2].strip() + '\t'
             + item_list[3].strip() + '\t'
             + stamp2time(float(item_list[4].strip())) + '\t'
             + stamp2time(float(item_list[5].strip())) + '\t'
             + item_list[6].strip() + '\n')
#    print content
    return content

#@brief parse the collect task item
#@param a line of collect_server content 'Recieve task'
def collect_task_item_parse(item):
    item_list = item.split(',') 
    content = (item_list[0].strip().split('client')[1].strip() + '\t'
            + item_list[1].strip().split(' ')[1] + '\t'
            + item_list[2].strip().split(' ')[2] + '\t'
            + stamp2time(float(item_list[3].strip().split(' ')[2])) + '\t'
            + stamp2time(float(item_list[4].strip().split(' ')[2].split('[')[0])) + '\t'
            + item_list[0].strip().split(']')[0].replace('[','') + '\n')
#    print content
    return content

#@brief parse the map log item
#@param a line of log line 
#Bug: sep by ',' 
"""
def task_url_item_parse(item):      
    item_list = item.split(',')
    task_seq = item_list[0].split('id')[1].strip()
    url_list = item_list[1].split('[')[0].strip().split(' ')
    content = ''
    for url in url_list:
        content = content + task_seq + '\t' + url + '\n'   
#    print content 
    return content
"""
def task_url_item_parse(item):
    head, _, main = item.partition(', ')
    _, __, task_seq = head.rpartition('id ')
    if len(task_seq.strip()) == 0:
        print task_seq, item
        exit(-1)
    content_list = []
    for url in main.split():
        if not url.startswith('http'):
            continue
        content_list.append(task_seq+'\t'+url)
    if len(content_list) != 0:
        return '\n'.join(content_list)+'\n'
    else:
        return ''

#@brief statistisc the log
#@param1 input file
#@param2 output file
#@param3 statisc type(collect_task | assign_task | spider_status | task_url)    
def log_statistics(input_file_name, output_file_name,statistisc_type,timetm):
    input_file_handle = open(input_file_name,'r');
    if not input_file_handle:
       print 'can not open file',input_file_name
       return None 
    output_file_handle = open(output_file_name,'a');
    if not output_file_handle:
       print 'can not open file',input_file_name
       return None
    #print 'extracting info from ' + input_file_name + ' extract_type = ' + statistisc_type + ' time:'+timetm
    
    file_list = input_file_handle.readlines()
    for file_line in file_list:
        output_line = ''
        if len(file_line) < 1:
            continue
        if statistisc_type !=  'spider_status':
            if not file_line.startswith('['+timetm):
                continue
            """
            try:
                if not timematch(file_line,timetm):
                    continue
            except:
                print input_file_name, file_line
                exit(-1)
            """
        if statistisc_type == 'collect_task':
            if 'Receive task' in file_line: 
#                      print file_line
                #  if timematch(file_line,timetm):
                      output_line =  collect_task_item_parse(file_line) 
                 # else: 
                 #     continue
        elif statistisc_type == 'spider_status':
             output_line = spider_status_item_parse(file_line)
        elif statistisc_type == 'assign_task':
             if 'Return task' in file_line:   
               output_line = assign_task_item_parse(file_line)
        elif statistisc_type == 'task_url':
             if 'Map task' in file_line:
               output_line = task_url_item_parse(file_line)
        else:
             print 'No this extract type!' 
             return None
        if len(output_line) > 0:
           output_file_handle.write(output_line)
#           print 'out_file:' + output_line
    input_file_handle.close()
    output_file_handle.flush()
    output_file_handle.close()

def filetra(dir):#is need file
    itemschedule=[]
    itemcollect=[]
    for item in os.listdir(dir):
        sitem=item.split('.')[0]
        if sitem=='schedule_server':
           #if timefilter(dir,item,timetm):
               itemschedule.append(dir+'/'+item)
        elif sitem=='collect_server':
           #if timefilter(dir,item,timetm):
               itemcollect.append(dir+'/'+item)  
    item={'schedule_server':itemschedule,'collect_server':itemcollect}
    return item#return dic

if __name__ == '__main__':
    if len(sys.argv) < 4:
       print 'Usage:\tpython'+sys.argv[0]+' date log-path output-path [table-name]' 
       print 'e.g.\t2013-12-08 /home/elvin.xu/fengyi /home/max.zhang/tmp [task_allocation | task_collect | allocation_task_url_map | collect_task_url_map | ALL]'
       exit(1)
    if len(sys.argv) == 4:
        date, log_path, output_path = sys.argv[1:]
        table_name = 'ALL'
    else:
        date, log_path, output_path, table_name = sys.argv[1:]
    hostname = os.popen('hostname').read().strip()
    
    if not os.path.exists(output_path):
        os.mkdir(output_path)
    # 列出schedule_server.log*和collect_server.log*文件路径
    file_list = filetra(log_path)

    task_allocation = os.path.join(output_path, 'task_allocation_'+hostname)
    task_collect = os.path.join(output_path, 'task_collect_'+hostname)
    allocation_task_url_map = os.path.join(output_path, 'allocation_task_url_map_'+hostname)
    collect_task_url_map = os.path.join(output_path, 'collect_task_url_map_'+hostname)

    table_name = table_name.strip()
    task_list = []
    if table_name == 'ALL':
        task_list = (['task_allocation', 'task_collect',
                      'allocation_task_url_map', 'collect_task_url_map'])
    else:
        task_list.append(table_name)

    if 'task_collect' in task_list:
        if os.path.isfile(task_collect):
           os.remove(task_collect)
        for collect_log_file in file_list['collect_server']:
            log_statistics(collect_log_file,task_collect,'collect_task',date)
    if 'collect_task_url_map' in task_list:
        if os.path.isfile(collect_task_url_map):
            os.remove(collect_task_url_map)
        for collect_log_file in file_list['collect_server']:
            log_statistics(collect_log_file,collect_task_url_map,'task_url',date)
    if 'task_allocation' in task_list:
        if os.path.isfile(task_allocation):
            os.remove(task_allocation) 
        for assign_log_file in file_list['schedule_server']:
            log_statistics(assign_log_file,task_allocation,'assign_task',date)
    if 'allocation_task_url_map' in task_list:
        if os.path.isfile(allocation_task_url_map):
           os.remove(allocation_task_url_map)
        for assign_log_file in file_list['schedule_server']:
            log_statistics(assign_log_file,allocation_task_url_map,'task_url',date)

