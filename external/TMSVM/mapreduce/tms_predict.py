#!/usr/ali/bin/python
# -*- coding: utf-8 -*-
import math 
import sys
import os
sys.path.append('.')
sys.path.append(os.path.join(os.path.dirname(__file__),"dependence"))
from hstream import *
import tms_predict_config 
import tms_svm
import measure
import segment
import re

def cal_sc_optim(lab,m,text,dic_list,local_fun,global_weight,str_splitTag):
    '''输入标签，模型，待预测的文本，词典，以及词分词用的符号
    返回的是一个预测标签与得分
    '''
    local_fun = measure.local_f(local_fun)
    y,x = cons_pro_for_svm(lab,text.strip().split(str_splitTag),dic_list,local_fun,global_weight)
    p_lab,p_acc,p_sc=tms_svm.predict(y,x,m)  
    return p_lab[0],tms_svm.classer_value(p_sc[0])

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
    return y,[x]

def ctm_predict_multi(text,indexes_lists,dic_list,model_list,local_fun_list,global_weight_list,str_splitTag):
    '''多个模型的预测，如一个文本有多个模型需要预测
    其中indexes_lists为二维度的。
    the main process to predict the text score.
    support deoloy more than one model.each model contains indexes,dic and model

    '''            
    k = len(dic_list) #得到预测模型的个数
    label_list =[0]*k
    score_list=[0]*k
   
    for j in range(k):
        indexes = indexes_lists[j]
        model = model_list[j]
        dic = dic_list[j]
        local_fun = local_fun_list[j]
        global_weight = global_weight_list[j]
        if len(text)<indexes[len(indexes)-1]+1 or len(text)<tms_predict_config.result_indexes[len(tms_predict_config.result_indexes)-1]+1:
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
        score_list[j]=float(sc)
        label_list[j]=float(label)
    return label_list,score_list

#read the needed dic and model
k = len(tms_predict_config.config_files) 
dic_list=[]
local_fun_list=[]
model_list=[]
global_weight_list=[]

for i in range(k):
    local_fun,dic,global_weight,model,seg_ori = tms_svm.load_tms_model(tms_predict_config.config_files[i]) 
    dic_list.append(dic)
    local_fun_list.append(local_fun)
    model_list.append(model)
    global_weight_list.append(global_weight)

class Post_Check(HStream):
    def mapper(self, key, value):
        text = value.strip().split(tms_predict_config.tc_splitTag)
        if tms_predict_config.seg>0:
            html_del = re.compile('<(.|\n)+?>|&nbsp;| |\t')
            all_index = list()
            for index in tms_predict_config.indexes_lists:
                all_index.extend(index)
            for i in range(len(text)): #对所有需要分词的指标项进行分词
                if all_index.count(i)>0:
                    text[i] = html_del.sub("",text[i])
                    text[i]=segment.word_seg(text[i],tms_predict_config.str_splitTag)
        if  len(text)>=tms_predict_config.result_indexes[len(tms_predict_config.result_indexes)-1]+1:        
            label_list,score_list = ctm_predict_multi(text,tms_predict_config.indexes_lists,dic_list,model_list,local_fun_list,global_weight_list,tms_predict_config.str_splitTag)          

            for i in range(len(label_list)):
                if label_list[i]>0 :
                    if tms_predict_config.thresholds[i].has_key(label_list[i]) :
                        if score_list[i]>tms_predict_config.thresholds[i][label_list[i]]:
                            output_value =""
                            for i in range(len(label_list)):
                                output_value +=tms_predict_config.tc_splitTag.join([str(label_list[i]),str(score_list[i])])+tms_predict_config.tc_splitTag 
                            for index in tms_predict_config.result_indexes:
                                output_value += text[index].strip()+ tms_predict_config.tc_splitTag 
                            if len(output_value.strip())>0:
                                print output_value
                            break

    def reducer(self,key,values):
        pass

if __name__ =="__main__":
    Post_Check()

#def predict(value):
#    text = value.strip().split(tms_predict_config.tc_splitTag)
#    if tms_predict_config.seg>1:
#        all_index = list()
#        for index in tms_predict_config.indexes_lists:
#            all_index.extend(index)
#        for i in range(len(text)): #对所有需要分词的指标项进行分词
#            if all_index.count(i)>0:
#                text[i]=segment.word_seg(text[i],tms_predict_config.str_splitTag)
#    if  len(text)>=tms_predict_config.result_indexes[len(tms_predict_config.result_indexes)-1]+1:        
#        label_list,score_list = ctm_predict_multi(text,tms_predict_config.indexes_lists,dic_list,model_list,local_fun_list,global_weight_list,tms_predict_config.str_splitTag)          
#
#        output_value =""
#        for i in range(len(label_list)):
#            output_value +=tms_predict_config.tc_splitTag.join([str(label_list[i]),str(score_list[i])])+tms_predict_config.tc_splitTag 
#        for index in tms_predict_config.result_indexes:
#            output_value += text[index]+ tms_predict_config.tc_splitTag 
#        return output_value
#
#
#filename ="../data/binary_seged.test"
#outputfile = file("../data/binary_seged.result",'w')
#for value in file(filename,'r'):
#    outputfile.write(predict(value)+"\n")
#outputfile.close()
