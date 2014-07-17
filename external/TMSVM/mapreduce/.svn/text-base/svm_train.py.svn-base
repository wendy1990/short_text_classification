#!/usr/bin/env python
#_*_ coding: utf-8 _*_
import sys
import os
sys.path.append('.')
from hstream import *
sys.path.append(os.path.join(os.path.dirname(__file__),"dependence"))
import tms_svm
'''
first step:先把整个程序流程跑通
seconde step:将输入参数文件和svm_type参数化。
'''
def read_param(filename):
    params=list()
    for line in file(filename):
        params.append(line.strip())
    return params

default_param_file="./params"
svm_type="libsvm"
params = read_param(default_param_file)

class SvmTrain(HStream):
    '''input 为SVM训练需要的数据'''

#    def __init__(self,param_file=default_param_file):
#        pass
#        self.parse_args()
#        print self.default_param_file
#        self.param_file=param_file
#        self.params = self.read_param(self.param_file)

    def mapper(self,key, value):
        '''mapper function'''
        for param in params:
            self.write_output( param, value )

    def reducer(self, key, values):
        '''reducer function'''
        prob_y=[]
        prob_x=[]
        line=key.split(None,1)
        if sum([1 for i in line])==1:
            svm_param = " -v 5 -c "+str(line[0])
        else :
            if sum([1 for i in line])>=2:
                svm_param = " -v 5 -c "+str(line[0])+" -g "+str(line[1])
        
        for value in values:
            value = value.split(None,1)
            if len(value)==1: value+=['']
            label, features = value
            xi={}
            for e in features.split():
                ind, val = e.split(":")
                xi[int(ind)] = float(val)
            prob_y +=[float(label)]
            prob_x +=[xi]
        tms_svm.set_svm_type(svm_type)
        ratio = tms_svm.train(prob_y,prob_x,svm_param)
        self.write_output( key, str(ratio))
    
#    def parse_args(self):
#        parser = OptionParser(usage="")
#        parser.add_option("-p", "--paramFile",help="param filename",dest="paramFile")
#        options,args = parser.parse_args()
#        if options.paramFile:
#            self.default_param_file=options.paramFile
        

    
if __name__ == '__main__':
    SvmTrain()
