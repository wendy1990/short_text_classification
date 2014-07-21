#!/user/bin/python
# -*- coding: utf-8 -*-
"""
module: feature_selection.py
usage: python feature_selection.py -i <inputfile> [-d  dic.key] [-s <stopwords file>] [-g globalfun]
desc: This is used to choose features
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
        help='已分词的训练样本名')
    parser.add_argument('-d', action='store', dest='dic_key',
        help='用户自定义特征词典名')
    parser.add_argument('-s', action='store', dest='stopword_file',
        help='停用词表路径')
    parser.add_argument('-g', action='store', dest='global_fun',
        help='全局权重选择')
    results = parser.parse_args()
    return results

def process(parse_result):
    '''
    desc: This function use to process features selection
    params: the instance returned by function--parser()
    return: None
    date: 2013-10-30
    '''
    input_file = parse_result.input_file.strip()
    if os.path.isfile(input_file): 
        global_fun='idf'
        dic_key = parse_result.dic_key.strip()
        stop_file=parse_result.stopword_file.strip()
        if parse_result.global_fun:
            global_fun =parse_result.global_fun.strip()
        # processing with not deleting stopwords
        if stop_file == None:
            tms.tms_feature_select(input_file,global_fun=global_fun,main_save_path='../data/',dic_name=dic_key,str_splitTag="^",ratio=0.4)
        else:
        # processing with deleting stopwords
            tms.tms_feature_select  (input_file,global_fun=global_fun,main_save_path='../data/',dic_name=dic_key,stopword_filename=stop_file,str_splitTag="^",ratio=0.4)
        print "第二步---<特征选择>的处理结果：\n"
        # Save the some training parameters
        with open('../data/model/post.config','a+') as f_config:
            lines=f_config.readlines()
            for line in lines: 
                if "DicName:" in line:         
                    break
                if line == lines[-1]:
                    f_config.write("DicName:"+str(dic_key).strip()+"\n")
            print "词典---DicName:"+str(dic_key).strip()+"---- was writen into:../data/model/post.config\n" 
            for line in lines: 
                if "GlobalFun:" in line:         
                    break
                if line == lines[-1]:     
                    f_config.write("GlobalFun:"+str(global_fun).strip()+"\n")
            print "全局权重---GlobalFun:"+str(global_fun).strip()+"---- was writen into:../data/model/post.config\n"
            print "词典保存的路径为：%s"%('../data/'+'model/'+dic_key)
    else:
        print "输入文件不存在！\n"    

if __name__ == "__main__":
    process(parser())

