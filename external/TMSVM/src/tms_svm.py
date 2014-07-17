#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author: 张知临 zhzhl202@163.com
#Filename: tms_svm.py
'''此处封装了libsvm与liblinear'''
import sys
import os
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__)),"dependence"))


import svm
import svmutil
import liblinear
import liblinearutil 
import math

read_problem = lambda x:x
train =  lambda x:x
predict =  lambda x:x
save_model =  lambda x:x
load_model = lambda x:x
svm_type=""
def set_svm_type(type):
    global read_problem,train,predict,save_model,load_model,svm_type
    svm_type = type
    if svm_type=="libsvm":
        read_problem = svmutil.svm_read_problem
        train = svmutil.svm_train
        predict = svmutil.svm_predict
        save_model = svmutil.svm_save_model
        load_model = svmutil.svm_load_model
            
    if svm_type=="liblinear":
        read_problem = liblinearutil.svm_read_problem
        train = liblinearutil.train
        predict = liblinearutil.predict
        save_model = liblinearutil.save_model
        load_model = liblinearutil.load_model
        
def detect_svm_type(model):
    f= open(model,'r')
    svm_type = ""
    for line in f.readlines():
        line = line.split()
        if line[0]=="svm_type":
            svm_type="libsvm"
            break
        if line[0]=="solver_type":
            svm_type="liblinear"
            break
    return svm_type

def classer_value(values):
    '''计算类得隶属度,libsvm采用的为one-against-one算法。
    liblinear采用的为oen-against-rest算法。因此在计算最终的隶属度分数上有所区别.
    计算公式为：sum(vi)/(2*k)+k/(2*n):n为所有参数类得总数,对libsvm为all-1,liblinear为1，k为支持该类的数,vi为支持该类的value
    '''
    
    if svm_type=="libsvm":
        n = 1+int(math.sqrt(2*len(values)+1))
        size = n-1
        vote=[0]*n
        score=[0]*n
        p=0
        for i in range(n-1):
            for j in range(i+1,n):
                if values[p]>0:
                    vote[i]+=1
                    score[i]+=math.fabs(values[p])
                else : 
                    vote[j]+=1
                    score[j]+=math.fabs(values[p])
                p+=1
        max = 0 
        for i in range(1,n):
            if vote[i]>vote[max]:
                max = i
        k = vote[max]
        init_score=score[max]
    if svm_type=="liblinear":
        n =len(values)
        max = 0
        for i in range(1,n-1):
            if values[i]>values[max]:
                max = i
        size = 1
        k = 1
        init_score = values[max]
    return float(init_score)/(2.0*k)+float(k)/(2.0*size) 


    
            


     