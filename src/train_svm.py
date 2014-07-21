#!/user/bin/python
# -*- coding: utf-8 -*-
"""
module: train_svm.py
usage:  python train_svm.py -i <libsvm-format file> -o <model_file> [-s svm_type] [-b probability or not]
desc: This is used to train a model
author: Felix Liang
date: 2013-10-30
"""
import sys
sys.path.append("../external/TMSVM/src")
import argparse
import os
import time
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
        help='libsvm格式文件名')
    parser.add_argument('-o', action='store', dest='dest_file',
        help='模型保存文件名')
    parser.add_argument('-s', action='store', dest='svm_selection',type=int,
        help='选择svm类型')
    parser.add_argument('-c', action='store', dest='config_file',
        help='配置文件')
    parser.add_argument('-b', action='store', dest='prob_or_not',type=int,
        help='选择训练的模型评估方式')
    results = parser.parse_args()
    return results

def process(parse_result):
    '''
    desc: This function is used to search the best training parameters
    params: the instance returned by function--parser()
    return: None
    date: 2013-10-30
    '''
    dest_file="../data/svm.model"
    input_file   = parse_result.input_file.strip()
    if os.path.isfile(input_file):
        print "Starting..."
        if parse_result.dest_file is not None:
            dest_file= parse_result.dest_file
        svm_selection=parse_result.svm_selection
        if svm_selection == 1:
            svm_type="libsvm"
        elif svm_selection == 2:
            svm_type="liblinear"
        b_param=''
        if parse_result.prob_or_not == 1:
            b_param=' -b 1'
        config='../data/model/post.config'
        cf = parse_result.config_file
        if cf is not None:
            conf = cf.strip()
            if conf is not None:
               config = conf
        with open(config,'r') as f:
            lines=f.readlines()
        for line in lines:
            if line.startswith('SvmParam'):
                c_g_param=line.split(':')[1].strip()
        f.close()
        modelname=dest_file.split('/')[-1].strip()   
        c_g_param=c_g_param.strip('\n')+b_param
        print c_g_param
        tms.tms_train_model(input_file,svm_type=svm_type,param=c_g_param,model_save_path=dest_file)
        with open(config,'a+') as f_config: 
            for line in lines: 
                if "LocalFun:" in line:         
                    break
                if line == lines[-1]:
                    f_config.write("LocalFun:tf"+"\n")
            for line in lines: 
                if "ModelName:" in line:         
                    break
                if line == lines[-1]:
                    f_config.write("ModelName:"+str(modelname).strip()+"\n")
    else:
        print "输入文件不存在！\n"

if __name__ == "__main__":
    process(parser())


