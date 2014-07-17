#!/usr/bin/env python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: predict_model.py

'''此文件主要存放一些预测所常用的函数'''

import train_model 
import tms_svm
import  fileutil
import os
import segment
import math
import measure
import ctmutil
import stem

#def cal_sc(lab,m,text,dic_list,str_splitTag):
#    '''输入标签，模型，待预测的文本，词典，以及词分词用的符号
#    返回的是一个浮点数
#    '''
#    vec = ctm_train_model.cons_vec_for_cla(text.strip().split(str_splitTag),dic_list)
#    y,x=cons_svm_problem(lab,vec)
#    p_lab,p_acc,p_sc=svm_predict(y,x,m)
#
#    return float(p_sc[0][0])

def cal_sc_optim(lab,m,text,dic_list,local_fun,global_weight,str_splitTag):
    '''输入标签，模型，待预测的文本，词典，以及词分词用的符号
    返回的是一个预测标签与得分，如果是二分类，返回的是直接得分，如果为多分类，返回的是经过计算的综合分数。
    '''
    local_fun = measure.local_f(local_fun)
    y,x = ctmutil.cons_pro_for_svm(lab,text.strip().split(str_splitTag),dic_list,local_fun,global_weight)
    p_lab,p_acc,p_sc=tms_svm.predict(y,x,m)  
    #在这里要判定是二分类还是多分类，如果为二分类，返回相应的分数，如果为多分类，则返回预测的标签。
    return p_lab[0],tms_svm.classer_value(p_sc[0])

def load_tms_model(config_file):
    '''通过模型配置文件加载词典、全局因子、局部因子、SVM模型'''
    model_main_path = os.path.dirname(config_file)
    f = file(config_file,'r')
    for line in f.readlines():
        text = line.split(":")
        if text[0].strip()=="DicName":
            dic,global_weight = fileutil.read_dic_ex(os.path.join(model_main_path,text[1].strip()),dtype=str)
        if text[0].strip()=="ModelName":
            tms_svm.set_svm_type(tms_svm.detect_svm_type(os.path.join(model_main_path,text[1].strip())))
            model= tms_svm.load_model(os.path.join(model_main_path,text[1].strip()))
        if text[0].strip()=="LocalFun":
            local_fun = measure.local_f(text[1].strip())
        if text[0].strip()=="WordSeg":
            seg = int(float(text[1]))
    return local_fun,dic,global_weight,model,seg
    
def ctm_predict(filename,config_file,indexes,result_save_path,result_indexes,str_splitTag,tc_splitTag,seg,delete=False,change_decode=False,in_decode="UTF-8",out_encode="GBK"):
    '''一般形式的下得模型预测，即单个模型。'''
    local_fun,dic,global_weight,model,seg_ori = load_tms_model(config_file) 
    if seg!=0:
        if seg_ori !=seg:
            print "预测分词工具与原训练时分词工具不一样"
        print "-----------------正在对源文本进行分词-------------------"
        segment_file = os.path.dirname(filename)+"/segmented"
        segment.file_seg(filename,indexes,segment_file,str_splitTag,tc_splitTag,seg)
        filename = segment_file
        
    print "-----------------正在对源文本进行词干化处理-------------------"
    stem.stemFile(filename,str_splitTag,tc_splitTag)
    
    f= file(filename,'r')
    fs = file(result_save_path,'w')
    print "-----------------正在对样本进行预测-------------------"
    for line in f.readlines():
        if change_decode ==True:
            line = line.decode(in_decode).encode(out_encode,'ignore')
        text = line.strip().split(tc_splitTag)
        if len(text)<indexes[len(indexes)-1]+1 or len(text)<result_indexes[len(result_indexes)-1]+1:
            continue       
        text_temp=""
        for i in indexes:
            text_temp+=str_splitTag+text[i]                   
        label,sc=cal_sc_optim(1,model,text_temp,dic,local_fun,global_weight,str_splitTag)
        fs.write(str(label)+"\t"+str(sc)+"\t")
        for index in result_indexes:
            fs.write(text[index]+"\t")
        fs.write("\n")
    f.close()
    fs.close()
    print "-----------------预测完毕-------------------"

def ctm_predict_multi(filename,config_files,indexes_lists,result_save_path,result_indexes,str_splitTag,tc_splitTag,seg,delete=False,change_decode=False,in_decode="UTF-8",out_encode="GBK"):
    '''多个模型的预测，如一个文本有多个模型需要预测
    其中title_indexes，dic_path ，model_path为二维度的。
    '''
    if seg!=0:
        print "-----------------正在对源文本进行分词-------------------"
        all_index = list()
        for index in indexes_lists:
            all_index.extend(index)
        segment_file = os.path.dirname(filename)+"/segmented"
        segment.file_seg(filename,all_index,segment_file,str_splitTag,tc_splitTag,seg)
        filename = segment_file
    k = len(config_files) #得到预测模型的个数
    dic_list=[]
    local_fun_list=[]
    model_list=[]
    global_weight_list = []
    for i in range(k):
        local_fun,dic,global_weight,model,seg_ori = load_tms_model(config_files[i]) 
        dic_list.append(dic)
        local_fun_list.append(local_fun)
        model_list.append(model)
        global_weight_list .append(global_weight)

    print "-----------------正在对源文本进行词干化处理-------------------"
    stem.stemFile(filename,str_splitTag,tc_splitTag)
            
    f= file(filename,'r')
    fs = file(result_save_path,'w')    
    print "-----------------正在对样本进行预测-------------------"
    for line in f.readlines():
        if len(line.strip())<1:
            continue
        if change_decode ==True:
            line = line.decode(in_decode).encode(out_encode,'ignore')
        text = line.strip().split(tc_splitTag)
        
        for j in range(k):
            indexes = indexes_lists[j]
            model = model_list[j]
            dic = dic_list[j]
            local_fun = local_fun_list[j]
            if len(text)<indexes[len(indexes)-1]+1 or len(text)<result_indexes[len(result_indexes)-1]+1:
                label =0
                sc=0  
            else:     
                text_temp=""
                for index in indexes:
                    text_temp+=str_splitTag+text[index]                   
                if dir(model).count("get_svm_type")==1:
                    tms_svm.set_svm_type("libsvm")
                if dir(model).count("get_nr_feature")==1:
                    tms_svm.set_svm_type("liblinear")
                label,sc=cal_sc_optim(1,model,text_temp,dic,local_fun,global_weight,str_splitTag)
            fs.write(str(label)+"\t"+str(sc)+"\t")
        for index in result_indexes:
            if index>len(text)-1:
                break
            fs.write(text[index]+"\t")
        fs.write("\n")
    f.close()
    fs.close()
    print "-----------------预测完毕-------------------"
#ctm_predict_multi("../model/binary.test",[[1],[1]],["../model/dic.key","../model/dic.key"],result_save_path,result_indexes,model_path_list,str_splitTag,tc_splitTag)
