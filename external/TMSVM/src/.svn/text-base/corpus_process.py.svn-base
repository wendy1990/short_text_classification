#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:’≈÷™¡Ÿ zhzhl202@163.com
#Filename: ctm_predict_model.py

import os
import re

def merge_with_label(f,files,label,tc_splitTag):
    for fi in files:
        fr = file(fi,'r')
        text = "".join(fr.readlines())
        p=re.compile("\s") #replace all the blank character ,such as \t \n 
        text = p.sub("",text)
        f.write(tc_splitTag.join([label,text]))
        f.write("\n")

def merge(f,files):
    for fi in files:
        fr = file(fi,'r')
        text = "".join(fr.readlines())
        p=re.compile("\s") #replace all the blank character ,such as \t \n
        text = p.sub("",text)
        f.write(text+"\n")
        
f = file("D:/sougou.corpus",'w')
paths = "D:/sougou"
for path_name in os.listdir(paths):
    basename = os.path.basename(paths+"/"+path_name)
    label = str(float(basename[1:]))
    files = os.listdir(paths+"/"+path_name)
    for i in range(len(files)):
        files[i]=paths+"/"+path_name+"/"+files[i]
    merge_with_label(f,files,label,"\t")
    
f.close()

    
