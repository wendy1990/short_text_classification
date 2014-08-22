#!/user/bin/python
# -*- coding: utf-8 -*-
"""
module: word_segment.py
usage: python word_segment.py -i <input_file> [-o <store_file>]
desc: This is used to segment text file
author: Wendy Yang
date: 2013-10-30
"""
import sys
import os
import time 
import argparse
sys.path.append("../external/TMSVM/src")
import tms

def parser():
    '''
    desc: This function use to get the command line pamameters
    params: None
    return: the pamameter object instance
    date: 2013-10-30
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', action='store', dest='source_file',
        help='待分词文件')
    parser.add_argument('-o', action='store', dest='dest_file',
        help='分词后保存文件')  
    parser.add_argument('-p', action='store', dest='p_flag',
        help='Training or predicting')     
    results = parser.parse_args()
    return results

def process(parse_result):
    '''
    desc: This function use to process the word segmentation
    params: the instance returned by function--parser()
    return: None
    date: 2013-10-30
    '''
    input_file = parse_result.source_file.strip()
    # The default outputfile to store the results
    output_file = "segmented.temp"
    p_flag=parse_result.p_flag
    if parse_result.dest_file is not None:
        output_file = parse_result.dest_file.strip()    
    if os.path.isfile(input_file): 
        print "Start......\n"
        # The key method to process the words segmentation
        tms.tms_segment(input_file,out_filename=output_file)
        print "第一步处理结果：\n"
        time.sleep(1)
        # Create a configuration file to store the trainning parameters
        if p_flag is not None:
            with open('../data/model/post.config','w') as f1_config:
                f1_config.write("-----Configuration-----"+"\n")
                f1_config.close()
        else :
            pass
        # Store the the method of word segments 
        with open('../data/model/post.config','a+') as f_config:
            lines=f_config.readlines()
            for line in lines: 
                if "WordSeg:" in line:         
                    break
                if line == lines[-1]:
                    f_config.write("WordSeg:"+str(1).strip()+"\n")
        if output_file == "segmented.temp":
            print "分词结果为:\n"
            time.sleep(1)
            with open(output_file,'r') as f:
                print f.read()        
        else:
            print "分词结果保存为: %s\n"%output_file
    else:
        print "输入文件不存在！\n"

if __name__ == "__main__":
    process(parser())

