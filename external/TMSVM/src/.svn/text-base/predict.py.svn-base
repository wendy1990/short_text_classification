#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author: 张知临 zhzhl202@163.com
import predict_model 
from optparse import *
import os 

def list_callback(option,opt,value,parser):
    setattr(parser.values,option.dest,value.split(','))

def main():
    usage="usage: %prog [options] filename config_path version= %prog 1.0"
    parser = OptionParser(usage=usage)
    parser.add_option("-i","--indexes",dest="indexes",action="callback",type="string",default=[0],callback=list_callback)
    parser.add_option("-r","--result_indexes",dest="result_indexes",action="callback",type="string",default=[0],callback=list_callback,help="specify the content indexes that output with the predicted score")
    parser.add_option("-R","--result_save",dest="result_save")
    parser.add_option("-e","--segment",type="choice",dest="segment",default=0,choices=[0,1,2])
    parser.add_option("-T","--tc_splitTag",dest="tc_splitTag",type="string",default="\t")
    parser.add_option("-S","--str_splitTag",dest="str_splitTag",type="string",default="^")
    options, args = parser.parse_args() 
    
    if options.indexes :
        indexes =[int(i) for i in options.indexes]
    if options.result_indexes:
        result_indexes  =[int(i) for i in options.result_indexes]
#    if options.save_main_path:
#        if os.path.exists(options.save_main_path):
#            if os.path.exists(options.save_main_path+"result/") is False:
#                os.mkdir(options.save_main_path+"result/")
#            result_save_path = options.save_main_path+"result/score.result"
#        else: result_save_path =os.getcwd()+"/score.result"
    filename = args[0]
    config_path = args[1]
    predict_model.ctm_predict(args[0], config_path, indexes, options.result_save, result_indexes, options.str_splitTag,options.tc_splitTag,options.segment)

if __name__ =="__main__":
    main()