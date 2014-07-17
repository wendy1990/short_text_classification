#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author: 张知临 zhzhl202@163.com
from ctm_predict_model import *
from optparse import *

def list_callback(option,opt,value,parser):
    setattr(parser.values,option.dest,value.split(','))

def ctm_predict_lsa(filename,indexes,dic_path,result_save_path,result_indexes,model_path,LSA_path,LSA_model_path,delete,str_splitTag,tc_splitTag,change_decode=False,in_decode="UTF-8",out_encode="GBK"):
    '''使用LSA模型进行预测的程序
    其过程为：首先使用普通的SVM模型进行打分，然后利用打分值对原向量做伸缩处理
    最后使用LSA模型进行预测得分。
    @param result_indexes:需要和评分结果一起存储的值。
    '''
    dic_list = read_dic(dic_path,dtype=str)
    f= file(filename,'r')
    fs = file(result_save_path,'w')
    m= svm_load_model(model_path)
    lsa_m = svm_load_model(LSA_model_path)
    U = load_lsa_model(LSA_path,"U")

    for line in f.readlines():
        if change_decode ==True:
            line = line.decode(in_decode).encode(out_encode,'ignore')
        text = line.strip().split(tc_splitTag)
        if len(text)<indexes[len(indexes)-1]+1 or len(text)<result_indexes[len(result_indexes)-1]+1:
            continue

        text_temp=""
        for i in indexes:
            text_temp+=str_splitTag+text[i]  
        vec = cons_vec_for_cla(text_temp.strip().split(str_splitTag),dic_list)
        y,x=cons_svm_problem(1,vec) #此处的lab 默认为1
        p_lab,p_acc,p_sc=svm_predict(y,x,m)
                
        weight = cal_weight(p_sc[0][0])
        vec = [0]*len(vec)
        for key in x[0].keys():
           vec[int(key)-1]= weight*float(x[0][key])
        vec = pre_doc_svds(vec,U)
        y,x=cons_svm_problem(text[0],vec)
        lsa_lab,lsa_acc,lsa_sc = svm_predict(y,x,lsa_m)
        fs.write(str(p_sc[0][0])+"\t"+str(lsa_sc[0][0])+"\t")
        for index in result_indexes:
            fs.write(text[index]+"\t")
        fs.write("\n")

    f.close()
    fs.close()


def main():
    usage="usage: %prog [options] filename dic_path model_path lsa_path lsa_model_path "
    parser = OptionParser(usage=usage)
    parser.add_option("-i","--indexes",dest="indexes",action="callback",type="string",default=[0],callback=list_callback)
    parser.add_option("-r","--result_indexes",dest="result_indexes",action="callback",type="string",default=[0],callback=list_callback,help="specify the content indexes that output with the predicted score")
    parser.add_option("-R","--result_save",dest="result_save")
    options, args = parser.parse_args() 
    
    if options.indexes :
        indexes =[int(i) for i in options.indexes]
    if options.result_indexes:
        result_indexes  =[int(i) for i in options.result_indexes]
    
    filename = args[0]
    dic_path = args[1]
    model_path = args[2]
    LSA_path = args[3]
    LSA_model_path = args[4]
    delete = False
    change_decode=False
    in_decode="UTF-8"
    out_encode="GBK"
    ctm_predict_lsa(filename,indexes,dic_path,options.result_save,result_indexes,model_path,LSA_path,LSA_model_path,delete=delete,tc_splitTag=options.tc_splitTag,str_splitTag=options.str_splitTag,change_decode=change_decode,in_decode=in_decode,out_encode=out_encode)

if __name__ =="__main__":
    main()