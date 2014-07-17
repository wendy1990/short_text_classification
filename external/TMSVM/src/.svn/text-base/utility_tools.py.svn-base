#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author: 张知临 zhzhl202@163.com
#Filename: utility_tools.py
'''
一些实用工具，包括
1、编码转换
2、将样本按照比例分割成训练样本和测试样本
3、
'''
import os.path
from random import *
from fileutil import *

def encode_trans(in_filename,in_decode,out_encode,out_filename=""):
    '''对文件的编码进行转换
    转换后的文件可以自己指定，或者是系统自己生成，在文件后面加上 "$out_enncode"
    '''
    f1= file(in_filename,'r')
    if len(out_filename.strip())==0:
        filename= in_filename.split(".")
        if len(filename)>=2:
            out_filename = ".".join(filename[0:len(filename)-1])+"_"+out_encode+"."+filename[len(filename)-1]
        else:
            out_filename=in_filename+"_"+out_encode
    f2=file(out_filename,'w')
    while True:
        line=f1.readline()
        if len(line)==0:
            break
        f2.write(line.decode(in_decode).encode(out_encode,'ignore'))
        
def sel_train_test_set(filename,train_filename,test_filename,all_num,ratio):
    '''将总的文本按照比例分割成训练文本和测试文本'''
    num_dic = dict()
    test_num= int(all_num*ratio)
    count=0
    while len(num_dic)<test_num:
        num_dic[randint(1,all_num)] = 1
    train_f = file(train_filename,'w')
    test_f = file(test_filename,'w')
    for line in file(filename,'r').readlines():
        count+=1
        if num_dic.has_key(count) is True:
            test_f.write(line)
        else:
            train_f.write(line)
    train_f.close()
    test_f.close()
 
def extract_random(filename,number,all_num,save_path):  
    '''从文件中随机读取$number条的记录放入到结果文件中。'''  
    num_dic = dict()
    count=0
    while len(num_dic)<number:
        num_dic[randint(1,all_num)] = 1
    fw= file(save_path,'w')
    for line in file(filename,'r').readlines():
        count+=1
        if num_dic.has_key(count) is True:
            fw.write(line)

    fw.close()

def extract_random_by_condition(filename,number,all_num,save_path): 
    '''从文件中随机读取$number条的满足条件的记录放入到结果文件中。''' 
    num_dic = dict()
    count=0
    while len(num_dic)<number:
        num_dic[randint(1,all_num)] = 1
    fw= file(save_path,'w')
    for line in file(filename,'r').readlines():
        part = line.split("\t")
        if part[0]=='normal' and float(part[7])<0.3 and float(part[7])>-1.2:
            count+=1
            if num_dic.has_key(count) is True:
                fw.write(line)
    fw.close()

def extract_by_lin_num(filename,line_num_path,save_path):
    '''根据输入的行号，从文件中抽取指定的行号的内容'''
    f= file(filename,'r')
    line_list = read_list(line_num_path,dtype=int)
    fw= file(save_path,'w')
    count=0
    for line in f.readlines():
        count+=1
        if line_list.count(count)>0:
            fw.write(line)
            line_list.remove(count)
    f.close()
    fw.close()
  
def extract_by_id(filename,id_num,id_dic,save_path):
    fw=file(save_path,'w')
    for line in file(filename,'r').readlines():
        part = line.split("\t")
        if id_dic.has_key(part[id_num]) is True:
            fw.write(str(id_dic[part[id_num]])+"\t")
            fw.write("\t".join(part[1:]))

    fw.close()


  
def split_by_lin_nums(filename,lin_nums,file_prefix,c_p):
    '''把一个大文件，按照行数平分为若干份，指定每个文件的行数'''
    f= file(filename,'r')
    count =0
    file_count=1
    all_count=1
    out_f = file(c_p+"data/20110820_all_data/"+file_prefix+"_"+str(file_count),'w')
    for line in f.readlines():
        line=line.decode('UTF-8').encode('gbk','ignore')
        all_count+=1
        if count==lin_nums:
            out_f.close()
            file_count+=1
            out_f= file(c_p+"data/20110820_all_data/"+file_prefix+"_"+str(file_count),'w')
            print "now system is editing the" +str(file_count)+"file"
            count=0
        out_f.write(line)
        count+=1
    out_f.close()
    print all_count


