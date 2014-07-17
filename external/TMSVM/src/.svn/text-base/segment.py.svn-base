#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: segment.py
import sys
import os
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__)),"../dependence"))

import pymmseg as seg

seg.dict_load_defaults()

def term_seg(text,str_splitTag,type):
    seg_text =""
    if type==1:
        #利用mmseg算法进行分词。
        line =  seg.Algorithm(text)
        for l in line:
            seg_text+=str(l)+str_splitTag
    return seg_text

def file_seg(filename,indexes,out_filename,str_splitTag,tc_splitTag,type):
    f = open(filename,'r')
    fw = open(out_filename,'w')
    for line in f.readlines():
        text = line.strip().split(tc_splitTag)
        for i in range(len(text)):
            if indexes.count(i)>0:
                fw.write(term_seg(text[i],str_splitTag,1)+tc_splitTag)
            else: fw.write(text[i]+tc_splitTag)
        fw.write("\n")
    fw.close()
    f.close()
        
        
        

def main():
    file_seg("../data/test.txt","../data/test_segmented.txt","^",1)
    f = file("../data/test.txt")
    text =f.readline()
    print term_seg(text,"^",1)

