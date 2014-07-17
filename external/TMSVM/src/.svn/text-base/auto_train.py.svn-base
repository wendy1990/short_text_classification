#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: auto_im_train.py
from optparse import OptionParser
import train_model
import os

'''
automatic training the svm model:
you must input :the training file(-f),specify the main path model saved (-p),the content indexes need to be trained(-i) 
    e.g: -f "../model/im_info/trainset_4000.train"  -i 6,7,8,9,10  -p "../model/im_info/"
and you can optional specify :whether use the stopword
the main step include: 
'''

def list_callback(option, opt, value, parser):
  setattr(parser.values, option.dest, value.split(','))

def main():
    usage ="usage:%prog [options] filename version=%prog 1.0"
    parser = OptionParser(usage=usage)
    #parser.add_option("-f","--file",dest="filename")
    parser.add_option("-p","--path",dest="save_main_path",default="../")
    parser.add_option("-i","--indexes",dest="indexes",action="callback",type="string",default=[1],callback=list_callback)
    parser.add_option("-w","--stopword",action="store_false",dest="stopword",default=True)
    parser.add_option("-n","--config_name",dest="config_name",default="tms.config")
    parser.add_option("-d","--dic_name",dest="dic_name",default="dic.key")
    parser.add_option("-m","--model_name",dest="model_name",default="tms.model")
    parser.add_option("-t","--train_name",dest="train_name",default="tms.train")
    parser.add_option("-a","--param_name",dest="param_name",default="tms.param")
    parser.add_option("-A","--svm_param",dest="svm_param",default="' '")
    parser.add_option("-r","--ratio",dest="ratio",type="float",default=0.4)
    parser.add_option("-T","--tc_splitTag",dest="tc_splitTag",type="string",default="\t")
    parser.add_option("-S","--str_splitTag",dest="str_splitTag",type="string",default="^")
    parser.add_option("-v","--svm_type",dest="svm_type",default="libsvm",type="choice",choices=["libsvm","liblinear"])
    parser.add_option("-e","--segment",type="choice",dest="segment",default=0,choices=[0,1,2])
    parser.add_option("-c","--param_select",action="store_false",dest="param_select",default=True)
    parser.add_option("-g","--global_fun",dest="global_fun",default="one",type="choice",choices=["one","idf","rf"])
    parser.add_option("-l","--local_fun",dest="local_fun",default="tf",type="choice",choices=["tf"])
    parser.add_option("-b","--label_file",dest="label_file",type="string",default="")
    options, args = parser.parse_args() 
    if options.indexes:
        indexes = [int(i) for i in options.indexes]
    if options.stopword == False:
        stopword_filename=""
    else:
        stopword_filename = os.path.dirname(args[0])+"/stopwords.txt"
    if options.svm_param:
        svm_param = options.svm_param.replace("'","")
    
    train_model.ctm_train(args[0],indexes,options.save_main_path,stopword_filename,config_name=options.config_name,svm_type =options.svm_type,seg=options.segment,param_select=options.param_select,global_fun=options.global_fun,local_fun=options.local_fun,svm_param=svm_param,dic_name=options.dic_name,model_name=options.model_name,train_name=options.train_name,param_name=options.param_name,ratio=options.ratio,delete=True,str_splitTag=options.str_splitTag,tc_splitTag=options.tc_splitTag,label_file=options.label_file)

if __name__ == "__main__":
    main()
