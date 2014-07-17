#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author: 张知临 zhzhl202@163.com
#Filename: result_analysis.py

'''从得到的结果中分析数据
统计分类的正确率Rate = correct/all
各个子类的F值、召回率、准确率
Macro,Micro

'''
from optparse import OptionParser
import types

def read_result(result_save_path,indexes,dtype=float):
    '''从文件中读取结果，给定文件中要读取的列表。返回一个矩阵。取第i列的元素使用x[:,i]'''
    f = file(result_save_path,'r')
    X=[]
    for line in f.readlines():
        arr = line.strip().split("\t")
        list =[]
        for i in indexes:
            list+=[float(arr[i])]
        X+=[list]
    f.close()
    return X

def cal_rate(true_lab,pre_lab):
    '''
    要求输入的值必须是list。array转list: arr.tolist()。
    返回的是总的micro,macro。
    '''   
    rate={}     
    if len(true_lab) != len(pre_lab):
        raise ValueError("len(true_lab) must equal to len(pre_lab)")
    total_correct  = 0.0
    for v, y in zip(pre_lab, true_lab):
        if y == v: 
            total_correct += 1
    l=len(true_lab)
    micro = float(total_correct)/l

    labels = {}.fromkeys(true_lab).keys()
    micro_acc = 0.0
    for la in labels:
        total_correct = 0.0
        for v, y in zip(pre_lab, true_lab):
            if y == v and y ==la: 
                total_correct += 1
        l=list(true_lab).count(la)
        micro_acc += total_correct/float(l)
        rate[la]=[total_correct/float(l)]
    macro = micro_acc/float(len(labels))
    
    return rate,micro,macro
             
def cal_f(true_lab,pre_lab):  
    '''要求输入的值必须是list。array转list: arr.tolist()。
    返回的是词典。对应着每个类的f_sc,recall,precision
    '''
    labels = {}.fromkeys(true_lab).keys()
    rate ={}
    for lab in labels:
        true_sum =0.0
        pre_sum=0.0
        right_sum=0.0
        f_sc,recall,precision=0.0,0.0,0.0
        for j in range(len(true_lab)):
            if true_lab[j]==lab:
                true_sum+=1.0
                if pre_lab[j]==true_lab[j]:
                    right_sum+=1.0
            if pre_lab[j]==lab:
                pre_sum+=1.0
        recall = right_sum/true_sum
        if pre_sum!=0:
            precision=right_sum/pre_sum
        if recall+precision!=0:
            f_sc = 2*recall*precision/(recall+precision)
        rate[lab]=[f_sc,recall,precision]
    return rate

def cal_f_by_threshold(true_lab,pre_lab,pre_value,label,threshold): 
    '''为指定的类别计算在当前阈值下的F值，召回率、准确率。并返回相应的词典，词典中存储了上述3个指标。'''
    true_sum =0.0
    pre_sum=0.0
    right_sum=0.0
    f_sc,recall,precision=0.0,0.0,0.0
    rate = dict()
    for j in range(len(true_lab)):
        if true_lab[j]==label:
            true_sum+=1.0
            if pre_lab[j]==label and pre_value[j]>=threshold:
                right_sum+=1.0
        if pre_lab[j]==label and pre_value[j]>=threshold:
            pre_sum+=1.0
    recall = right_sum/true_sum
    if pre_sum!=0:
        precision=right_sum/pre_sum
    if recall+precision!=0:
        f_sc = 2*recall*precision/(recall+precision)
    rate[label]=[f_sc,recall,precision] 
    return rate


def threshlod_anlysis(true_lab,pre_lab,pre_value,first_range=[i/10.0 for i in range(10)]):
    '''对每个阈值进行分析，计算其F值、召回率、准确率'''
    rate = {}
    labels = {}.fromkeys(true_lab).keys()
    for label in labels:
        threshold_rate=dict()
        for threhold in first_range:
           threshold_rate[threhold]=cal_f_by_threshold(true_lab,pre_lab,pre_value,label,threhold)[label]
        rate[label]=threshold_rate
    return rate


def save_result(f,rate_dic,count=0):
    '''将结果词典写入到已经打开的文件中。
    系统将探测变量类型。此处为了保证写入的格式好看，利用了递归程序。
    每一级会记录上一级所
    '''
    if type(rate_dic)==types.DictType:
        f.write("\t")
        temp=count
        for key in sorted(rate_dic.keys()):
            f.write("\n")
            f.write("\t"*temp+str(key))
            count=temp
            count+=1
            save_result(f,rate_dic[key],count)     
    else:
        if type(rate_dic) in(types.ListType,types.TupleType):
            f.write("\t"*count)
            for value in rate_dic:
                f.write(str(value)+"\t")      
        else:
            f.write("\t"*count)
            f.write(str(rate_dic)+"\t")

def print_result(rate_dic,count=0):
    '''将结果词典写入到已经打开的文件中。
    系统将探测变量类型。此处为了保证写入的格式好看，利用了递归程序。
    每一级会记录上一级所
    '''
    if type(rate_dic)==types.DictType:
        print "\t"
        temp=count
        for key in sorted(rate_dic.keys()):
            print "\n"
            print "\t"*temp+str(key)
            count=temp
            count+=1
            print_result(rate_dic[key],count)     
    else:
        if type(rate_dic) in(types.ListType,types.TupleType):
            print "\t"*count
            for value in rate_dic:
                print str(value)+"\t"      
        else:
            print "\t"*count
            print str(rate_dic)+"\t"

def do_statis(f,original_res,new_sc,text,second_range):
    f.write("--------------------------"+str(text)+"------------------------------"+"\n")
    f.write("阈值\t平均值\n")
    for threhold in second_range:
        mean,quan_label,quan_list=res_statistics(original_res,new_sc,threhold)
        f.write(str(threhold)+"\t"+str(mean)+"\n")
        f.write("fraction\t百分位值\n")
        for i  in range(len(quan_label)):
            f.write("\t"+str(quan_label[i])+"\t"+str(quan_list[i])+"\n")
            
def statis_with_result(result_save_path,statistics_result_save_path,first_range=[x/10.0 for x in range(0,10)],second_range=[0,0.5]):
    X=read_result(result_save_path)
    X=array(X)
    f= file(statistics_result_save_path,'w')
    cal_affect(f,X[:,1],X[:,0],"标题",first_range)
    #cal_affect(f,X[:,0],X[:,2],"内容",first_range)
    cal_affect(f,X[:,2],X[:,0],"标题+内容",first_range)
    
    do_statis(f,X[:,1],X[:,0],"标题",second_range)
    #do_statis(f,X[:,0],X[:,2],"内容",second_range)
    do_statis(f,X[:,2],X[:,0],"标题+内容",second_range)
    f.close()

def grid_search(result_save_path,best_param_result_save_path,par1_range=[x/10.0 for x in range(-9,10)],par2_range=[x/10.0 for x in range(-9,10)]):
    X=read_result(result_save_path,[0,1,2])
    X=array(X) 
    f= file(best_param_result_save_path,'w')
    best_par,all_parm=search_best_param(X[:,2],X[:,0],par1_range,X[:,1],par2_range)
    print "最优的参数为：( "+str(best_par[0])+" , "+str(best_par[1])+" )，其各项指标为：F="+str(best_par[2])+",Recall="+str(best_par[3])+",Precision="+str(best_par[4])+"\n"
    for i in range(len(all_parm)):
        for j in range(len(all_parm[i])):
            f.write(str(all_parm[i][j])+"\t")
        f.write("\n")

def big_data_statis(result_save_path,statistics_result_save_path,first_range=[x/10.0 for x in range(-5,10)],second_range=[0,0.5]):
    '''对大样本进行统计分析，分析器召回率、准确率以及F值。'''
    X=read_result(result_save_path,[0,1,2])
    X=array(X)
    
    f= file(statistics_result_save_path,'w')
    big_data_cal_affect(f,X[:,0],X[:,2],"对于标题+内容",first_range,374)

    f.close()



def big_data_res_analysis(original_res,new_sc,threhold,ill_count):
    '''对大规模样本进行测试，其中总的违规样本个数已知，需给出计算召回率
    正确率就是疑似违规中，找出真正违规的比例
    original_res 即每个样本真正类别，
    new_sc 即得分
    threhold 即选择的得分阈值
    ill_count 即所有违规样本的总数
    '''
    rf_c = ill_count #系统中原有的违规样本总数
    tt_c = 0 #从违规样本中找出个数
    tt_all_c = 0#所有判断为违规样本的个数 
    
    for i in range(len(original_res)):

        if int(original_res[i].strip())==1:
          if  float(new_sc[i])>threhold:
               tt_c+=1
        if  float(new_sc[i])>threhold: 
            tt_all_c +=1
    recall = float(tt_c)/float(rf_c)
    if tt_all_c==0:
        precise=0
    else:
        precise = float(tt_c)/float(tt_all_c)
    if recall+precise==0:
        f_sc=0
    else:
        f_sc = 2*recall*precise/(recall+precise)
    return recall,precise,f_sc,tt_all_c,tt_c
 
def big_data_cal_affect(f,original_res,new_sc,text,first_range,ill_count):
    f.write("--------------------------"+str(text)+"------------------------------"+"\n")
    f.write("阈值\tF值\t召回率\t正确率=\t共找到疑似违规帖子\t真正违规\t漏掉\n")
    for threhold in first_range:
       recall,precise,f_sc,tt_all_c,tt_c=big_data_res_analysis(original_res,new_sc,threhold,ill_count)
       f.write(str(threhold)+"\t"+str(f_sc)+"\t"+str(recall)+"\t"+str(precise)+"\t"+str(tt_all_c)+"\t"+str(tt_c)+"\t"+str(ill_count-tt_c)+"\n")

def res_anlysis_two(original_res,pra_1_list,tr_1,par_2_list,tr_2):
    rf_c = 0 #系统中原有的违规样本总数
    tt_c = 0 #从违规样本中找出个数
    tt_all_c = 0#所有判断为违规样本的个数 
    
    for i in range(len(original_res)):

        if int(original_res[i].strip())==1:
          rf_c +=1
          if  float(pra_1_list[i])>tr_1 and float(par_2_list[i])>tr_2:
               tt_c+=1
        if  float(pra_1_list[i])>tr_1 and float(par_2_list[i])>tr_2: 
            tt_all_c +=1
    recall = float(tt_c)/float(rf_c)
    if tt_all_c==0:
        precise=0
    else:
        precise = float(tt_c)/float(tt_all_c)
    if recall+precise==0:
        f_sc=0
    else:
        f_sc = 2*recall*precise/(recall+precise)
    return recall,precise,f_sc

def res_statistics(original_res,new_sc,threhold,quantile=[0.05,0.1,0.2,0.5,0.8,0.9]):
    '''make a statistics to the positive result.threhlod is the diving line between positive and negative
    some indices: mean 5% 10% 20% 50% 80% 90% 
    
    '''
    count =0
    sum=0.0
    list = []
    
    for i  in range(len(new_sc)):
        if int(original_res[i])==1 and float(new_sc[i])>threhold:
            count+=1
            sum+=float(new_sc[i])
            list +=[new_sc[i]]
    list.sort()
    quan = []
    for fraction in quantile:
        quan +=[list[int(count*fraction)]]
        
    return float(sum)/float(count),quantile,quan
  
def search_best_param(original_res,pra_1_list,par1_range,par_2_list,par2_range): 
     best_par = [0.0,0.0,0.0,0.0,0.0] #best_par1,best_par2,f_sc,recall,precision
     all_par = []
     for tr_1 in par1_range:
         for tr_2 in par2_range:
             recall,precise,f_sc=res_anlysis_two(original_res,pra_1_list,tr_1,par_2_list,tr_2)
             all_par.append([tr_1,tr_2,f_sc,recall,precise])
             if f_sc>best_par[2]:
                 best_par=[tr_1,tr_2,f_sc,recall,precise]
     return best_par,all_par

def list_callback(option,opt,value,parser):
    setattr(parser.values,option.dest,value.split(','))

def main():
    '''function 是值要选择进行分析的函数，其中1为cal_multi_rate（多类别模型的分类准确率）；2为cal_f（）'''
    ""
    "cal_f"
    "cal_f_binary"
    usage="usage: %prog [options] filename "
    parser = OptionParser(usage=usage)
    parser.add_option("-s","--step",dest="step",type="int",default=3)
    parser.add_option("-i","--indexes",dest="indexes",action="callback",type="string",default=[0,1,2],callback=list_callback)
    parser.add_option("-p","--predicted_label_index",dest="predicted_label_index",type="int",default=0)
    parser.add_option("-v","--predicted_value_index",dest="predicted_value_index",type="int",default=1)
    parser.add_option("-t","--true_label_index",dest="true_label_index",type="int",default=2)
    parser.add_option("-e","--threshold",dest="threshold",type="float",default=0)
    parser.add_option("-l","--label",dest="label",type="float",default=1.0,help="")
    parser.add_option("-o","--output",dest="output",type="str",default="",help="输出位置，默认为标准屏幕输出")
    parser.add_option("-m","--min",dest="min",type=int,default=0,help="设定阈值的最小范围")
    parser.add_option("-M","--max",dest="max",type="int",default=1,help="设定阈值的最大范围")
    
    
    options, args = parser.parse_args() 
    filename = args[0]
    if options.indexes:
        indexes = [int(i) for i in options.indexes]
    output =False
    if len(options.output.strip())>0:
        output =True
        f = file(options.output,'w')
    min = options.min*10
    max = options.max*10
    X = read_result(filename,indexes)
    
    if options.step==1:
        rate,micro,macro = cal_rate([y[options.true_label_index] for y in X ],[y[options.predicted_label_index] for y in X ])
        if output==False:
            print "micro = %g,macro = %g" %(micro,macro)
            print rate
        else:
            f.write("micro = %g,macro = %g\n" %(micro,macro))
            f.write("各个类别的分类准确率")
            save_result(f,rate)
            
    if options.step==2:
        rate  = cal_f([y[options.true_label_index] for y in X ],[y[options.predicted_label_index] for y in X ])
        if output==False:
            print rate
        else:
            f.write("各个类别的F值、召回率、准确率")
            save_result(f,rate)

    if options.step==3:
        rate  = cal_f_by_threshold([y[options.true_label_index] for y in X ],[y[options.predicted_label_index] for y in X ],[y[options.predicted_value_index] for y in X],options.label,options.threshold)
        if output==False:
            print rate
        else:
            f.write("对特定类别的F值、召回率、准确率")
            save_result(f,rate)
            
    if options.step==4:
        rate = threshlod_anlysis([y[options.true_label_index] for y in X ],[y[options.predicted_label_index] for y in X ],[y[options.predicted_value_index] for y in X],first_range=[i/10.0 for i in range(min,max)])
        if output==False:
            print rate
        else:
            f.write("所有类别各个阈值下得F值、召回率、准确率")
            save_result(f,rate)
        
    if output ==True:
        f.close()

if __name__=="__main__":
    main()
        
        
   

    