#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: train_model.py
'''此文件转为训练分类器模型，读入的文件格式
Label    value1 [value2...]#即第一个为类标签，第二个为内容，中间用Tab隔开
'''
#from ctm_train_model_config import *
import math
import tms_svm
import segment
import fileutil
import measure
import ctmutil 
import types
from feature_select import feature_select
import grid_search_param 
import os
import time
import stem

def ctm_train(filename,indexes,main_save_path,stopword_filename,svm_param,config_name,dic_name,model_name,train_name,svm_type,param_name,ratio,delete,str_splitTag,tc_splitTag,seg,param_select,global_fun,local_fun,label_file):
    '''训练的自动化程序，分词,先进行特征选择，重新定义词典，根据新的词典，自动选择SVM最优的参数。
    然后使用最优的参数进行SVM分类，最后生成训练后的模型。
    需要保存的文件：（需定义一个主保存路径）
                 模型文件：词典.key+模型.model
                临时文件 ：svm分类数据文件.train
    filename 训练文本所在的文件名
    indexs需要训练的指标项
    main_save_path 模型保存的路径
    stopword_filename 停用词的名称以及路径 ;默认不适用停用词
    svm_type :svm类型：libsvm 或liblinear
    svm_param  用户自己设定的svm的参数,这个要区分libsvm与liblinear参数的限制；例如"-s 0 -t 2 -c 0.2 "
    dic_name 用户自定义词典名称;例如“dic.key”
    model_name用户自定义模型名称 ;例如"svm.model"
    train_name用户自定义训练样本名称 ；例如“svm.train”
    param_name用户自定义参数文件名称 ；例如"svm.param"
    ratio 特征选择保留词的比例 ；例如 0.4
    delete对于所有特征值为0的样本是否删除,True or False
    str_splitTag 分词所用的分割符号 例如"^"
    tc_splitTag训练样本中各个字段分割所用的符号 ，例如"\t"
    seg 分词的选择：0为不进行分词；1为使用mmseg分词；2为使用aliws分词
    param_select ;是否进行SVM模型参数的搜索。True即为使用SVM模型grid.搜索，False即为不使用参数搜索。
    local_fun：即对特征向量计算特征权重时需要设定的计算方式:x(i,j) = local(i,j)*global(i).可选的有tf,logtf
    global_fun :全局权重的计算方式：有"one","idf","rf"
    label_file:类标签的解释说明文件。
    '''

    print "-----------------创建模型文件保存的路径-----------------"
    if os.path.exists(main_save_path):
        if os.path.exists(os.path.join(main_save_path,"model")) is False:
            os.makedirs(os.path.join(main_save_path,"model"))
    if os.path.exists(main_save_path):
        if os.path.exists(os.path.join(main_save_path,"temp")) is False:
            os.makedirs(os.path.join(main_save_path,"temp"))
    
    #设定SVM模型的类型。  
    
    tms_svm.set_svm_type(svm_type)   
        
    #如果没有给出停用词的文件名，则默认不使用停用词
    if stopword_filename =="":
        stop_words_dic=dict()
    else:
        stop_words_dic = fileutil.read_dic(stopword_filename)
    
    #如果需要分词，则对原文件进行分词
    if seg!=0:
        print "-----------------正在对源文本进行分词-------------------"
        segment_file = os.path.dirname(filename)+"/segmented"
        segment.file_seg(filename,indexes,segment_file,str_splitTag,tc_splitTag,seg)
        filename = segment_file
    
    #对原训练样本进行词干化处理
    print "-----------------正在对源文本进行词干化处理-------------------"
    stem.stemFile(filename,str_splitTag,tc_splitTag)
    
    print "-----------------现在正在进行特征选择---------------"  
    dic_path= os.path.join(main_save_path,"model",dic_name)    
    feature_select(filename,indexes,global_fun,dic_path,ratio,stop_words_dic,str_splitTag=str_splitTag,tc_splitTag=tc_splitTag)
    
    print "-----------------再根据特征选择后的词典构造新的SVM分类所需的训练样本------------------- "
    problem_save_path  = os.path.join(main_save_path,"temp",train_name)
    local_fun_str = local_fun
    local_fun = measure.local_f(local_fun)
    label = cons_train_sample_for_cla(filename,indexes,local_fun,dic_path,problem_save_path,delete,str_splitTag,tc_splitTag)
    
    if param_select ==True:
        print"--------------------选择最优的c,g------------------------------"
        search_result_save_path  = main_save_path +"temp/"+param_name
        if svm_type=="libsvm":
           coarse_c_range=(-5,7,2)
           coarse_g_range=(3,-10,-2)
           fine_c_step=0.5
           fine_g_step=0.5
           c,g=grid_search_param.grid(problem_save_path,search_result_save_path,svm_type,coarse_c_range,coarse_g_range,fine_c_step,fine_g_step)
           svm_param = svm_param + " -c "+str(c)+" -g "+str(g)
        if svm_type=="liblinear" or (svm_type=="libsvm" and is_linear_kernal(svm_param) is True):
           coarse_c_range=(-5,7,2)
           coarse_g_range=(1,1,1)
           fine_c_step=0.5
           fine_g_step=0
           c,g=grid_search_param.grid(problem_save_path,search_result_save_path,svm_type,coarse_c_range,coarse_g_range,fine_c_step,fine_g_step)
           svm_param = svm_param + " -c "+str(c)
    
    print "-----------------训练模型，并将模型进行保存----------"
    model_save_path  = main_save_path+"model/"+model_name
    ctm_train_model(problem_save_path,svm_type,svm_param,model_save_path)
    
    print "-----------------保存模型配置-----------------"
    f_config = file(os.path.join(main_save_path,"model",config_name),'w')
    save_config(f_config,dic_name,model_name,local_fun_str,global_fun,seg,svm_type,svm_param,label_file,label)
    f_config.close()

def save_config(f,dic_name,model_name,local_fun,global_fun,seg,svm_type,svm_param,label_file,label):
    '''保存模型配置文件'''
    f.write("SvmType:"+str(svm_type).strip()+"\n")
    f.write("SvmParam:"+str(svm_param).strip()+"\n")
    f.write("DicName:"+str(dic_name).strip()+"\n")
    f.write("ModelName:"+str(model_name).strip()+"\n")
    f.write("LocalFun:"+str(local_fun).strip()+"\n")
    f.write("GlobalFun:"+str(global_fun).strip()+"\n")
    f.write("WordSeg:"+str(seg).strip()+"\n")
    f.write("Date:"+time.strftime("%Y-%m-%d-%H-%M-%S",time.localtime(time.time()))+"\n")
    f.write("Labels:\n{\n") #将类标签写入，类标签会以"label,descr"进行存储
    if label_file.strip()!="":
        for line in file(label_file):
            f.write(line+"\n")
    else :
        for l in label:
            f.write(str(int(l))+",\n")
    f.write("}\n")

def ctm_feature_select(filename,indexes,global_fun,main_save_path,dic_name,ratio,stopword_filename,str_splitTag,tc_splitTag):
    #如果模型文件保存的路径不存在，则创建该文件夹
    dic_path= main_save_path+"model/"+dic_name
    if os.path.exists(main_save_path):
        if os.path.exists(main_save_path+"model/") is False:
            os.makedirs(main_save_path+"model/")
    #如果没有给出停用词的文件名，则默认不使用停用词
    if stopword_filename =="":
        stop_words_dic=dict()
    else:
        stop_words_dic = fileutil.read_dic(stopword_filename)
    feature_select(filename,indexes,global_fun,dic_path,ratio,stop_words_dic,str_splitTag,tc_splitTag)

def cons_train_sample_for_cla(filename,indexs,local_fun,dic_path,sample_save_path,delete,str_splitTag,tc_splitTag):
    '''根据提供的词典，将指定文件中的指定位置上的内容构造成SVM所需的问题格式，并进行保存'''
    dic_list,global_weight = fileutil.read_dic_ex(dic_path,dtype=str)
    if type(local_fun)==types.StringType:
        local_fun = measure.local_f(local_fun)
    label = set()
    #对原训练样本进行词干化处理
    print "-----------------正在对源文本进行词干化处理-------------------"
    stem.stemFile(filename,str_splitTag,tc_splitTag)    
    
    f= file(filename,'r')
    fs = file(sample_save_path,'w')
    for line in f.readlines():
        text = line.strip().split(tc_splitTag)
        text_temp=""
        if len(text)<indexs[len(indexs)-1]+1:
            continue
        for i in indexs:
          text_temp+=str_splitTag+text[i]  
        y,x = ctmutil.cons_pro_for_svm(text[0],text_temp.strip().split(str_splitTag),dic_list,local_fun,global_weight)
        if delete == True and len(x[0])==0:
            continue
        save_dic_train_sample(fs,y,x)
        label.add(y[0])
    f.close()
    fs.close()
    return label

def extract_im_feature(filename,content_indexs,feature_indexs,dic_path,svm_model,delete,str_splitTag,tc_splitTag):
    ''''''
    m = tms_svm.load_model(svm_model)
    f = file(filename,'r')
    for line in f.readlines():
        text = line.strip().split(tc_splitTag)
        text_temp=""
        for i in content_indexs:
          text_temp+=str_splitTag+text[i]  
          p_lab,p_acc,p_sc =tms_svm.predict() 

def file_seg(filename,indexes=[1],out_filename="",str_splitTag="^",tc_splitTag="\t",seg=1):
    if out_filename=="":
       out_filename = os.path.dirname(filename)+"/segmented"
    segment.file_seg(filename, indexes, out_filename, str_splitTag, tc_splitTag, type)

def save_dic_train_sample(f,y,x):
    '''将构造的svm问题格式进行保存
    y为list x为list里面为 词典。[ {} ]
    '''
    for i in range(len(y)):
        f.write(str(y[i]))
        #将字典有序的输出来。
        #sorted(dic.items(),key=lamda dic:dic[0],reverse = False)
        #dic =x[0]
        #sorted_keys=sorted(dic.items(),key=lambda dic:dic[0],reverse=False)
        sorted_keys = x[i].keys()
        sorted_keys.sort()
        for key  in sorted_keys:
            f.write("\t"+str(key)+":"+str(x[i][key]))
        f.write("\n")

def is_linear_kernal(svm_param):
		'''判断参数中是否包含线性核函数'''
		index = svm_param.find("-t")
		if index<0:
			return False
		for i in  range(index+2,len(svm_param)):
			if svm_param[i]!=' ' and svm_param[i]!='0':
				return False
			if svm_param[i]=='0':
				return True
		return False

def save_list_train_sample(f,lab,vec):
    '''将特征向量以SVM的输入格式进行保存，
    lab 为str，vec为list
    '''
    f.write(lab)
    for i  in range(len(vec)):
        if vec[i]!=0:
            f.write("\t"+str(i+1)+":"+str(vec[i]))
    f.write("\n")

def ctm_train_model(sample_save_path,svm_type,param,model_save_path):
    '''训练模型，输入样本文件，训练的参数，模型的保存地址，最后会给出模型在训练样本上的测试结果。'''
    tms_svm.set_svm_type(svm_type)
    y,x = tms_svm.read_problem(sample_save_path)
    m = tms_svm.train(y,x,param)
    tms_svm.save_model(model_save_path,m)
    labels = {}.fromkeys(y).keys()
    if len(labels)>2:
        pred_labels, (Micro, Macro, ACC), pred_values = tms_svm.predict(y,x,m)
        print "(Micro=%g, Macro=%g, ACC=%g)"%(Micro, Macro, ACC)
    else:
        pred_labels, (f_score,recall,presion), pred_values=tms_svm.predict(y,x,m)
        print "(f_score=%g,recall=%g,presion=%g)"%(f_score,recall,presion)
    return m

def ctm_model_predict(test_path,m):
    '''模型预测，输入测试样本，然后读入进行测试'''
    y,x = tms_svm.read_problem(test_path)
    return tms_svm.predict(y,x,m)



#def add_sample_to_model(extra_filename,indexs,dic_path,sample_save_path,delete,str_splitTag,tc_splitTag):
#    '''将之前误判的样本，放入到样本中重新训练。'''
#    dic_list = fileutil.read_dic(dic_path,dtype=str)
#    #glo_aff_list = read_list(glo_aff_path)
#    f= file(extra_filename,'r')
#    fs = file(sample_save_path,'a')
#    for line in f.readlines():
#        text = line.strip().split(tc_splitTag)
#        text_temp=""
#        for i in indexs:
#          text_temp+=str_splitTag+text[i]  
#        y,x = ctmutil.cons_pro_for_svm(text[0],text_temp.strip().split(str_splitTag),dic_list)
#        if delete == True and len(x)==0:
#            continue
#        save_dic_train_sample(fs,y,x)
#    f.close()
#    fs.close()




#def main():
#    filename = "D:/张知临/源代码/python_ctm/model/im_info/10.15_30000/temp/im.train"
#    svm_model_path = "D:/张知临/源代码/python_ctm/model/im_info/10.15_30000/model/im.model"
#    M = 8368
#    main_save_path = "D:/张知临/源代码/python_ctm/model/im_info/10.15_30000/"
#    threshold = 1.0 
#    K = 50
#    for_lsa_train = "for_lsa.train"
#    train_name = "lsa.train"
#    svm_param = " "
#    param_name = "lsa.param"
#    model_name = "lsa.model"
#    lsa_svm_train(filename,svm_model_path,M,main_save_path,threshold,K,svm_param,for_lsa_train,train_name,param_name,model_name)


