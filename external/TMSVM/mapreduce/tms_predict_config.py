#!/usr/bin/ali/python
# -*- coding: utf-8 -*-

#Filename: post_check_new_config.py
import os.path
import time
import sys
sys.path.append('.')

#split tag between id,title,content,dd1,dd2
#tc_splitTag="\t"
tc_splitTag=""
#split tag between single term after segment by Aliws
str_splitTag ="^"
#if seg>0 indicate seging the text before predict
seg =1
# specify the each model config files path
#config_files = ["./model/lineartitle.config","./model/lineartitle_content.config","./model/svmtitle.config","./model/svmtitle_content.config"]
#indexes_lists = [ [2],[2,3],[2],[2,3]]

config_files = ["./model/chaoxin_linear_1212.config"]
indexes_lists = [ [2,3]]
result_indexes=[0,1,2,3,4]

#indexes_lists = [ [4],[4,5],[4],[4,5] ]
#result_indexes=[0]

#模型的阈值，如果得分超过了阈值，则输出预测结果。
#通常在模型训练时会把正常文本设定为-1，所以这里只输出label>0且阈值>threshold的数据。
thresholds = [{1:0.5,2:0.9,3:0.9,4:0.8,5:0.9,6:0.9,7:0.9,8:1.0,9:0.5,10:0.5,11:0.5,12:0.5,13:0.5,14:0.5,15:0.5}] 
