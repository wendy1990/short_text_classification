#!/user/bin/python
# -*- coding: utf-8 -*-
"""
module: cons_trainsample_for_libsvm.py
usage: python cons_trainsample_for_libsvm.py -i <input_file> [-d dict_name] [-o <output_file>]
desc: This is used to construct the libsvm-format file
author: Felix Liang
date: 2013-10-30
"""
import sys
import os
sys.path.append("../../../../external/TMSVM/src")
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
        help='已分词的训练样本名')
    parser.add_argument('-d', action='store', dest='dic_key',
        help='用户自定义特征词典名')
    parser.add_argument('-o', action='store', dest='dest_file',
        help='结果保存位置')
    results = parser.parse_args()
    return results

def process(parse_result):
    '''
    desc: This function use to construct the standard format file and save some parameters
    params: the instance returned by function--parser()
    return: None
    date: 2013-10-30
    '''
    input_file = parse_result.input_file.strip()
    if os.path.isfile(input_file): 
        dic_key= parse_result.dic_key.strip()
        dest_file=parse_result.dest_file
        tms.cons_train_sample_for_svm(input_file,dic_key,sample_save_path=dest_file,str_splitTag="^")
        print "第三步<libsvm格式构造>处理结果：\n"
        print "构造好的libsvm格式文件为：%s\n\n\n"%dest_file
    else:
        print "输入文件不存在！\n"

if __name__ == "__main__":
    process(parser())

