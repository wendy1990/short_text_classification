#!/user/bin/python
# -*- coding: utf-8 -*-
"""
module: cons_trainsample_for_libsvm.py
usage:  python svm_c_g_param.py -i <libsvm-format file> -o <output_file> [-s svm_type]
desc: This is used to search the best 'c' value and 'g' value
author: Felix Liang
date: 2013-10-30
"""
import sys
import os
sys.path.append("../external/TMSVM/src")
import argparse
import tms
import time

def parser():
    '''
    desc: This function use to get the command line pamameters
    params: None
    return: the pamameter object instance
    date: 2013-10-30
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', action='store', dest='input_file',
        help='libsvm格式文件名')
    parser.add_argument('-o', action='store', dest='dest_file',
        help='结果文件名')
    parser.add_argument('-c', action='store', dest='config_file',
        help='配置文件名')
    parser.add_argument('-s', action='store', dest='svm_selection',type=int,
        help='选择svm类型')
    results = parser.parse_args()
    return results

def process(parse_result):
    '''
    desc: This function is used to search the best training parameters
    params: the instance returned by function--parser()
    return: None
    date: 2013-10-30
    '''
    # The default outputfile to store the results of parameters searching
    dest_file="../data/svm.param"
    input_file = parse_result.input_file.strip()
    config_file = "../data/model/post.config"
    cf = parse_result.config_file
    if cf is not None:
       conf=cf.strip()
       if conf is not None:
         config_file = conf
    if parse_result.dest_file is not None:
        dest_file= parse_result.dest_file.strip()
    svm_selection=parse_result.svm_selection
    if svm_selection == 1:
        svm_type="libsvm"
    elif svm_selection == 2:
        svm_type="liblinear"
    if os.path.isfile(input_file): 
        print "Start...\n"
        time.sleep(1)
        # The key method of search the best 'c' and 'g'
        tms.tms_grid_param(input_file, result_save_path=dest_file, svm_type=svm_type, coarse_c_range=(-5,7,2), coarse_g_range=(3,-10,-2), fine_c_step=0.5, fine_g_step=0.5)
        print "第四步---<SVM参数搜索>的处理结果：\n"
        ff=file(dest_file,'r')
        lines =ff.readlines()
        ff.close()
        # Store the best 'c' and 'g' to configuration file
        line=lines[-1]
        patch=line.split(',')
        c_value=patch[0].split('=')[-1].strip()
        g_value=patch[1].split('=')[-1].strip()
        param_list="-t 2 " + "-c " + str(c_value) + " -g " + str(g_value)
  
        with open(config_file,'w') as f_config:
            f_config.write("SvmType:"+str(svm_type).strip()+"\n")
            print "svm参数---'SvmType:'+ %s---- was writen into:../data/model/post.config\n"%str(svm_type).strip()
            f_config.write("SvmParam:"+str(param_list).strip()+"\n")
            print "svm参数---'SvmParam:'+ %s---- was writen into:../data/model/ post.config\n"%str(param_list).strip()
            f_config.close()
        print "结果文件写入到: %s\n\n\n"%dest_file
    else:
        print "输入文件不存在！\n"

if __name__ == "__main__":
    process(parser())

