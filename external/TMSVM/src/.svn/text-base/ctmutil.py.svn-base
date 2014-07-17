#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: ctmutil.py


from random import *
import measure
import math 
import os.path


c_p = os.path.dirname(os.getcwd())+"/"
#tc_splitTag="\t"
#str_splitTag = "^" #分词分割的标记

def cons_pro_for_svm(label,text,dic,local_fun=measure.tf,global_weight=dict()):
    '''根据构造的输入的类标签和以及经过分词后的文本和词典，SVM分类所用的输入格式，会对特征向量进行归一化
        注意：这个实现已经去除了全局因子的影响，意味着特征权重直接使用词频。
    x begin from 1'''
    y=[float(label)]
    x={}
    real_x={} #因为x的keys可能是无序的，所以要先对x中的进行排序，然后
    if len(global_weight)<1:
        for i in range(len(dic)+1):
            global_weight[i]=1
             
    #构造特征向量
    for term in text:
        term  = term.strip()
        if dic.has_key(term) :
            index = int(dic.get(term))
            if x.has_key(index):
                x[index]+=1.0
            else:
                x[index]=1.0
    # 计算特征向量的特征权重
    for key in x.keys():
        x[key] = local_fun(x[key])*global_weight.get(key)
        
    #计算特征向量的模
    vec_sum = 0.0
    for key in x.keys():
        if x[key]!=0:
            vec_sum+=x[key]**2.0
    #对向量进行归一化处理。
    vec_length=math.sqrt(vec_sum)
    if vec_length!=0:
        for key in x.keys():
            x[key]=float(x[key])/vec_length
    #sorted_keys=sorted(dic.items(),key=lambda dic:dic[0],reverse=False) 
#    sorted_keys = x.keys()
#    sorted_keys.sort()
#    for key in sorted_keys:
#        real_x[key]=x[key]
    return y,[x]

def cons_vec_for_cla(text,dic,glo_aff_list=[],normalization=1):
    '''给定词典、全局因子，对文本构造特征向量。需要设定是否需要对向量进行归一化
    vector 从0开始算起。
    '''
    vec = [0]*(len(dic))
    if len(glo_aff_list)==0: #如果不输入全局因子，则默认情况下就为1，即词频。
        glo_aff_list=[1]*(len(dic)) 
    #string = text.strip().split(str_splitTag)
    for term in text:
        term  = term.strip()
        if dic.has_key(term) :
            index = int(dic.get(term))
            #vec[index]=math.log(string.count(term)+1)*glo_aff_list[index]
            vec[index-1]+=1
    if normalization ==1:
        #表示需要对向量进行归一化
        temp_dic={}
        vec_sum=0.0       
        for i in range(len(vec)):
            if vec[i]!=0:
                temp_dic[i]=vec[i]*glo_aff_list[i]
                vec_sum+=temp_dic[i]**2
        #对向量进行归一化处理。
        vec_length=math.sqrt(vec_sum)
        if vec_length!=0:
            for key in temp_dic.keys():
                vec[int(key)]=float(temp_dic[key])/vec_length
    else: #不需要对向量进行归一化
        for i in range(len(vec)):
            if vec[i]!=0:
                vec[i]=vec[i]*glo_aff_list[i]
    return vec

def cons_svm_problem(lab,vec):
    '''构造svm的问题格式，lab为标签，1或者-1.vec为一个list'''
    y=[float(lab)]
    x={}
    for i in range(len(vec)):
        if vec[i]!=0:
            x[i+1]=float(vec[i])
    return y,[x]


