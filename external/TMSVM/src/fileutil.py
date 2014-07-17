#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: fileutil.py
'''读写文件的一些工具
包括读写list或二维list
'''


def save_list(filename,li):
    f = file(filename,'w')
    for i in range(len(li)):
        f.write(str(li[i])+"\n")
    f.close()
    
def save_twoDim_list(filename,li):
    f = file(filename,'w')
    for i in range(len(li)):
        for j in range(len(li[i])):
            f.write(str(li[i][j])+"\t")
        f.write("\n")
    f.close()
def read_list(filename,dtype=float):
    f= file(filename,'r')
    li=[]
    while True:
        line=f.readline()
        if len(line)==0:
            break
        li.append(dtype(line.strip()))
    f.close()
    return li


def read_dic(filename,dtype=str):
    '''这里的dic的是包括2列的，：term + index，用tab分割'''
    f= file(filename,'r')
    dic={}
    count=0
    for line in f.readlines():
        line=line.split("\t")
        count+=1
        if len(line)<1:
            continue
        if len(line)==1:
            dic[dtype(line[0].strip())]=count
        else:
            dic[dtype(line[0].strip())]=int(float(line[1]))

    f.close()
    return dic

def read_dic_ex(filename,dtype=str):
    '''这里的dic的是包括3列的，：term + index + global_weight，用tab分割
    dic的主键是index,
    '''
    f= file(filename,'r')
    dic={}
    global_weight ={}
    count=0
    for line in f.readlines():
        line=line.split("\t")
        count+=1
        if len(line)<1:
            continue
        if len(line)==2:
            dic[dtype(line[0].strip())]=count
            global_weight[int(float(line[1]))] = 1
        else:
            dic[dtype(line[0].strip())]=int(float(line[1]))
            global_weight[int(float(line[1]))] = float(line[2])

    f.close()
    return dic,global_weight


'''下面两个函数为读写矩阵，使用时需要引入numpy的包
from numpy import *
'''
#def save_arr(f,arr):
#    '''将矩阵保存在制定的文件中
#    此处的f为一个file类型
#    '''
#    m,n = arr.shape
#    for i in range(m):
#        for j in range(n):
#            f.write(str(arr[i][j])+"\t")
#        f.write("\n")
#def read_arr(filename,dtype=float):
#    tempX=[]
#    f = file(filename,'r')
#    while True:
#        line=f.readline()
#        if len(line)==0:
#            break
#        tempX.append(line.strip().split('\t'))
#
#    X = empty_like(tempX,dtype=dtype)
#    for i in range(len(tempX)):
#        for j in range(len(tempX[i])):
#            X[i][j]=dtype(tempX[i][j])
#    return X
#    
