#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author: 张知临 zhzhl202@163.com
from optparse import OptionParser
import os
import tms_svm
import lsa

def save_train_for_lsa(test_path,model_save_path,lsa_train_save_path):
    '''predict trainset using the initial classifier  ,and save the trainset with
    lsa format : label score feature
    '''
    y,x = tms_svm.read_problem(test_path)
    m = tms_svm.load_model(model_save_path)
    p_lab,p_acc,p_sc = tms_svm.predict(y,x,m)
    f= file(lsa_train_save_path,'w')
    for i  in range(len(y)):
        f.write(str(int(y[i]))+"\t"+str(p_sc[i][0])+"\t")
        dic =x[i]
        sorted_x = sorted(dic.items(),key = lambda dic:dic[0])
        for key in sorted_x:
            f.write(str(key[0])+":"+str(key[1])+"\t")
        f.write("\n")
    f.close()   
    
def lsa_svm_train(filename,svm_model_path,M,main_save_path,threshold,K,svm_param,for_lsa_train,train_name,param_name,model_name):
    '''此处的filename为libsvm的格式，训练普通的模型是放置在temp中train文件。'''
    if os.path.exists(main_save_path):
        if os.path.exists(main_save_path+"lsa/") is False:
            os.makedirs(main_save_path+"lsa/")
        if os.path.exists(main_save_path+"model/") is False:
            os.makedirs(main_save_path+"model/")
        if os.path.exists(main_save_path+"temp/") is False:
            os.makedirs(main_save_path+"temp/")
    print"--------------------使用SVM模型预测训练文本，为LSA模型准备输入------------------------------"
    for_lsa_train_save_path = main_save_path +"temp/"+for_lsa_train
    save_train_for_lsa(filename,svm_model_path,for_lsa_train_save_path)
    print"--------------------构造LSA模型------------------------------"
    lsa_train_save_path = main_save_path +"temp/"+train_name
    lsa_save_path = main_save_path +"lsa/lsa"
    lsa.ctm_lsa(M,threshold,K,for_lsa_train_save_path,lsa_train_save_path,lsa_save_path)
    
    print"--------------------选择最优的c,g------------------------------"
    search_result_save_path  = main_save_path +"temp/"+param_name
    if svm_type=="libsvm":
       coarse_c_range=(-5,7,2)
       coarse_g_range=(3,-10,-2)
       fine_c_step=0.5
       fine_g_step=0.5
       c,g=grid(problem_save_path,search_result_save_path,svm_type,coarse_c_range,coarse_g_range,fine_c_step,fine_g_step)
       svm_param = svm_param + " -c "+str(c)+" -g "+str(g)
    if svm_type=="liblinear":
       coarse_c_range=(-5,7,2)
       coarse_g_range=(1,1,1)
       fine_c_step=0.5
       fine_g_step=0
       c,g=grid(problem_save_path,search_result_save_path,svm_type,coarse_c_range,coarse_g_range,fine_c_step,fine_g_step)
       svm_param = svm_param + " -c "+str(c)
    
    print "-----------------根据得到的最优参数，训练模型，并将模型进行保存----------"
    svm_param = svm_param + " -c "+str(c)+" -g "+str(g)
    model_save_path  = main_save_path+"model/"+model_name
    ctm_train_model(lsa_train_save_path,svm_param,model_save_path)

def add_sample_to_model_lsa(extra_filename,indexs,dic_path,glo_aff_path,sample_save_path,model_path,LSA_path,LSA_model_path,delete,str_splitTag,tc_splitTag):
    '''将之前误判的样本，放入到LSA样本中重新训练。'''
    dic_list = read_list(dic_path,dtype=str)
    glo_aff_list = read_list(glo_aff_path)
    f= file(extra_filename,'r')
    fs = file(sample_save_path,'a')
    m= svm_load_model(model_path)
    lsa_m = svm_load_model(LSA_model_path)
    U = load_lsa_model(LSA_path,"U") 
    for line in f.readlines():
        text = line.strip().split(tc_splitTag)
        text_temp=""
        for i in indexs:
          text_temp+=str_splitTag+text[i]  
        #y,x = cons_pro_for_svm(text[0],text_temp.strip().split(str_splitTag),dic_list)
        vec = cons_vec_for_cla(text_temp.strip().split(str_splitTag),dic_list,glo_aff_list)
        y,x=cons_svm_problem(text[0],vec)
        p_lab,p_acc,p_sc=svm_predict(y,x,m)
        if delete == True and len(vec)==vec.count(0):
            continue
        weight = cal_weight(p_sc[0][0])
        vec = [0]*len(vec)
        for key in x[0].keys():
           vec[int(key)-1]= weight*float(x[0][key])
        vec = pre_doc_svds(vec,U)
        save_list_train_sample(fs,text[0],vec)
    f.close()
    fs.close()


def main():
    usage ="usage:%prog [options] filename svm_model M "
    parser = OptionParser(usage=usage)
    parser.add_option("-p","--path",dest="save_main_path",default="../")
    parser.add_option("-e","--threshold",dest="threshold",type="float",default=1.0)
    parser.add_option("-K","--K",dest="K",type="int",default=500)
    parser.add_option("-f","--for_lsa_train",dest="for_lsa_train",default="for_lsa.train")
    parser.add_option("-t","--train_name",dest="train_name",default="lsa.train")
    parser.add_option("-m","--model_name",dest="model_name",default="lsa.model")    
    parser.add_option("-A","--svm_param",dest="svm_param",default="'-s 0 -t 2 '")
    parser.add_option("-a","--param_name",dest="param_name",default="lsa.param")

    options, args = parser.parse_args() 
    if options.svm_param:
        svm_param = options.svm_param.replace("'","") 
    filename = args[0]
    svm_model = args[1]
    M = int(args[2])
    lsa_svm_train(filename,svm_model,M,options.save_main_path,options.threshold,options.K,svm_param,options.for_lsa_train,options.train_name,options.param_name,options.model_name)

if __name__ =="__main__":
    main()