#!/user/bin/python
# -*- coding: utf-8 -*-
"""
module: predict_post.py
usage:  python analysis_result.py -i <predict-result_file> [-o <result-analysis-file>]
desc: This is used to analysis the predict results
author: Felix Liang
date: 2013-10-30
"""
import sys
import os
sys.path.append("../external/TMSVM/src")
import argparse
import tms


def parser():
    '''
    desc: This function use to get the command line pamameters
    params: None
    return: the pamameter object instance
    date: 2013-10-30
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', action='store', dest='input_file',
        help='模型预测结果文件')
    parser.add_argument('-o', action='store', dest='dest_file',
        help='结果分析文件')
    results = parser.parse_args()
    return results

def process(parse_result):
    '''
    desc: This function is used to process the analyzing of results
    params: the instance returned by function--parser()
    return: None
    date: 2013-10-30
    '''
    dest_file="../data/results.analysis"
    input_file = parse_result.input_file.strip()
    if os.path.isfile(input_file):
        flag=0
        if parse_result.dest_file:
            dest_file=parse_result.dest_file.strip()
            flag=1
        tms.tms_analysis(input_file,step=2,output_file=dest_file,indexes=[0,1,2],predicted_label_index=0,predicted_value_index=1,true_label_index=2)
        if flag==0:
            with open('../data/results.analysis') as f:
                print f.read()
    else:
        print "输入文件不存在！\n"    

if __name__ == "__main__":
    process(parser())  
        
