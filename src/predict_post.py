#!/user/bin/python
# -*- coding: utf-8 -*-
"""
module: predict_post.py
usage:  python predict_post.py -i <test_file> [-o <predict-result file>] [-b probability or not]
desc: This is used to predict the model
author: Felix Liang
date: 2013-10-30
"""
import sys
sys.path.append("../../../../external/TMSVM/src")
import os
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
        help='测试样本名')
    parser.add_argument('-o', action='store', dest='dest_file',
        help='预测结果文件名')
    parser.add_argument('-b', action='store', dest='prob_or_not',type=int,
        help='预测结果文件名')
    results = parser.parse_args()
    return results

def process(parse_result):
    '''
    desc: This function is used to predict the model by input some test-file
    params: the instance returned by function--parser()
    return: None
    date: 2013-10-30
    '''
    dest_file="../data/tms.result"
    input_file = parse_result.input_file.strip()
    param=0
    if parse_result.prob_or_not is not None:
        param = parse_result.prob_or_not
    if parse_result.dest_file is not None:
        dest_file=parse_result.dest_file
    if os.path.isfile(input_file): 
        print "Start......\n"
        tms.tms_predict_("../data/model/post.config",input_file,dest_file,param)
        print "第六步---<模型预测>的结果：\n"
        print "预测结果文件保存为:%s\n\n\n"%dest_file
    else:
        print "输入文件不存在！\n"    

if __name__ == "__main__":
    process(parser())  
