#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: measure.py
'''此处实现了各种处理函数'''
import math
import types

tf = lambda x: x
logtf = lambda x: math.log(x)
def binary(x):
    if x<0:
        return 1
    return 0

def local_f(fun_type):
    if type(fun_type)==types.StringType:
        if fun_type=="tf":
            return tf
        if fun_type =="logtf":
            return logtf
        if fun_type=="binary":
            return binary
    if type(fun_type)==types.FunctionType:
        return fun_type
    
def global_f(fun_type):
    fun_type = fun_type.strip()
    if type(fun_type)==types.StringType:
        if fun_type=="one":
            return one
        if fun_type=="idf":
            return idf
        if fun_type=="rf":
            return rf
        if fun_type=="chi":
            return chi
    if type(fun_type)==types.FunctionType:
        return fun_type
    
def idf(dic,cat_num_dic,rows):
    '''返回每个词的idf值，计算公式为 log(N/n+1)'''
    global_weight =dict()
    for key in dic.keys():
        n = sum(dic[key].values())
        a = math.log(float(rows)/(n+1.0))
        if a<0:
            print "hello"
        global_weight[key] = math.log(float(rows)/(n+1.0))
    return global_weight

def rf(dic,cat_num_dic,rows):
    '''rf = log(2+a/c)'''
    global_weight=dict()
    for term in dic.keys():
        term =term.strip()
        rf_score= 0.0
        all_num_term = float(sum(dic[term].values()))
        for cat in cat_num_dic.keys():
            A  =  float(dic[term][cat]) #既在该类也包含该term
            C = float(all_num_term-A) # 不在该类但包含该term
	    rf_score = max(rf_score,math.log(2+A/max(1,C)))
        global_weight[term] = rf_score
    return global_weight

def one(dic,cat_num_dic,rows):
    '''指词典中所有词的权重都为1'''
    global_weight =dict()
    for key in dic.keys():
        n = sum(dic[key].values())
        global_weight[key] = 1
    return global_weight

def chi(dic,cat_num_dic,rows):
    '''利用卡方公式计算每个term的分值'''
    term_score=dict()
    for term in dic.keys():
        term =term.strip()
        all_num_term = float(sum(dic[term].values()))
        all_num_cat = float(sum(cat_num_dic.values()))
        chi_score= 0.0
        for cat in cat_num_dic.keys():
            A  =  float(dic[term][cat]) #既在该类也包含该term
            B= float(cat_num_dic[cat]-A) #在该类中但不包含该term
            C = float(all_num_term-A) # 不在该类但包含该term
            D = float(all_num_cat-all_num_term-cat_num_dic[cat]+A) #既不包含term也不在该类中。
            if (A+C)*(B+D)*(A+B)*(C+D) ==0:
                chi_score=0
            else:
                chi_score = max(chi_score,rows*math.pow(A*D-B*C,2)/((A+C)*(B+D)*(A+B)*(C+D)))
        term_score[term]=chi_score
    return term_score
