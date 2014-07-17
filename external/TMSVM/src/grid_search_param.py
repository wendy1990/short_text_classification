#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: grid_search_param.py

'''利用grid方法搜索svm分类的最优参数'''
import tms_svm
from random import randint
import math
import time

def grid(problem_path,result_save_path,svm_type,coarse_c_range,coarse_g_range,fine_c_step,fine_g_step):
    '''搜索的主文件;
    svm_type :使用的模型的类型。"libsvm"或者"liblinear"
    coarse_c_range :粗粒度c搜索的范围，为一个truple (begin,end,step)
    coarse_g_range :粗粒度g搜索的范围，为一个truple (begin,end,step)
    fine_c_step :细粒度c搜索的步长，搜索范围为(fine_c-coarse_c_step,fine_c+coarse_c_step,fine_c_step),如果为0，则固定为(fine_c,fine_c,fine_c)
    fine_g_step :细粒度g搜索的步长，搜索范围为(fine_g-coarse_g_step,fine_g+coarse_g_step,fine_g_step),如果为0，则固定为(fine_g,fine_g,fine_g)
    '''
    tms_svm.set_svm_type(svm_type)
    y,x  = tms_svm.read_problem(problem_path)
    fw= file(result_save_path,'w')
    c,g=grid_search_for_large_data(y,x,fw,coarse_c_range,coarse_g_range,fine_c_step,fine_g_step)  
    fw.close()
    return c,g

def range_f(begin,end,step):
    ''''like range, but works on non-integer too'''
    seq = []
    while True:
        if step > 0 and begin > end: break
        if step < 0 and begin < end: break
        seq.append(begin)
        begin = begin + step
    return seq

def permute_sequence(seq):
    '''将原先的列表中的次序打乱'''
    n = len(seq)
    if n <= 1: return seq

    mid = int(n/2)
    left = permute_sequence(seq[:mid])
    right = permute_sequence(seq[mid+1:])

    ret = [seq[mid]]
    while left or right:
        if left: ret.append(left.pop(0))
        if right: ret.append(right.pop(0))

    return ret

def calculate_jobs(c_range,g_range):
    '''根据C和G的列表，随意进行组合(C,G)pair'''
    c_seq = permute_sequence(c_range)
    g_seq = permute_sequence(g_range)
    nr_c = float(len(c_seq))
    nr_g = float(len(g_seq))
    i = 0
    j = 0
    jobs = []

    while i < nr_c or j < nr_g:
        if i/nr_c < j/nr_g:
            # increase C resolution
            for k in range(0,j):
                jobs.append((c_seq[i],g_seq[k]))
            i = i + 1
        else:
            # increase g resolution
            for k in range(0,i):
                jobs.append((c_seq[k],g_seq[j]))
            j = j + 1
    return jobs

def coarse_grid(y,x,fw,c_range,g_range):
    '''粗粒度的grid，一般默认的步长为2，c范围是(-5,7,2)， g范围是(3, -15, -2)'''
    fw.write("-------------现在进行粗粒度的grid，Log2C=(%s %s %s),Log2G=(%s %s %s)----------\n" %\
             (min(c_range),max(c_range),float(max(c_range)-min(c_range))/float(max(1,len(c_range)-1)),min(g_range),max(g_range),float(max(g_range)-min(g_range))/float(max(len(g_range)-1,1))))
    return grid_search(y,x,fw,c_range,g_range)

def better_region_grid(y,x,fw,c_range,g_range):
    '''细粒度的grid  一般来讲，是在选定的c,g的周围的一个步长范围内，新的步长为0.5'''
    fw.write("-------------现在进行细粒度的grid，Log2C=(%s %s %s),Log2G=(%s %s %s)-----------\n" %\
             (min(c_range),max(c_range),float(max(c_range)-min(c_range))/float(max(len(c_range)-1,1)),min(g_range),max(g_range),float(max(g_range)-min(g_range))/float(max(len(g_range)-1,1))))
    return grid_search(y,x,fw,c_range,g_range)

def grid_search(y,x,fw,c_range,g_range):
    '''c,g从的变化
    c_begin, c_end, c_step = -5,  15, 2
    g_begin, g_end, g_step =  3, -15, -2
    '''
    best_rate,best_c,best_g = 0.0,2**-5,2**-15
    esp = 0.001*100 #如果rate的增长幅度不超过esp，且c比best_c大时，就不会更新best_c
    job = calculate_jobs(permute_sequence(c_range),permute_sequence(g_range))
    fw.write("需要试验的组合有%s个：\n"%(len(job)))
    for (c1,g1) in job:
        fw.write(str(2**c1)+"\t"+str(2**g1)+"\n")
    fw.write("下面是各种组合得到的交叉验证的效果：\n")
    for (c1,g1) in job:
        c,g=2**c1,2**g1
        param="-v 5 "
        if len(c_range)>2:
            param+=" -c "+str(c)
        if len(g_range)>2:
            param+=" -g "+str(g)
        rate  = tms_svm.train(y,x,param)
        if (c < best_c and rate > best_rate-esp ) or (c > best_c and rate-esp > best_rate) or (c== best_c and rate > best_rate) or (abs(rate-best_rate)<esp and g==best_g and c<best_c):
            best_rate = rate
            best_c,best_g=c,g
        fw.write(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))+"\t")
        fw.write("%s\t%s\t%s\t(best c = %s,g = %s,rate =%s)\n" %(c,g,rate,best_c,best_g,best_rate))
        fw.flush()
        print "%s %s %s (best c = %s,g = %s,rate =%s)\n" %(c,g,rate,best_c,best_g,best_rate)
    fw.write("(best c = %s,g = %s,rate =%s)\n" %(best_c,best_g,best_rate))
    return best_c,best_g

def grid_search_for_large_data(y,x,fw,coarse_c_trupele,coarse_g_trupele,fine_c_step,fine_g_step):
    '''对与大型的数据集，可以先随机选取子集做grid_search,然后有了better region 后，再来做
    grid search
    '''
    coarse_c_range =range_f(coarse_c_trupele[0],coarse_c_trupele[1],coarse_c_trupele[2])
    coarse_g_range =range_f(coarse_g_trupele[0],coarse_g_trupele[1],coarse_g_trupele[2])
    sub_y=[]
    sub_x=[]
    l = len(y)
    
    #子集的大小应该是根据原训练样本的大小来决定的，其容量应该初步设定为原训练样本的40%，且不超过5000，不低于3000。如果总样本的容量本来就小于2000，则就不用再选子集了。
    subset_num=min(5000,max(min(3000,len(y)),int(len(y)*0.4)))
    sub_y,sub_x=subset(y,x,subset_num)
    
    c,g = coarse_grid(sub_y,sub_x,fw,coarse_c_range,coarse_g_range)
    c,g=math.log(c,2),math.log(g,2)
    
    coarse_c_step = math.fabs(coarse_c_trupele[2])
    coarse_g_step = math.fabs(coarse_g_trupele[2])
    if fine_c_step ==0:
        fine_c_range = range_f(c,c,c)
    else:
        fine_c_range = range_f(c-coarse_c_step,c+coarse_c_step,fine_c_step)
    if fine_g_step ==0:
        fine_g_range = range_f(g,g,g)
    else:
        fine_g_range = range_f(g-coarse_g_step,g+coarse_g_step,fine_g_step)
        
    return better_region_grid(y,x,fw,fine_c_range,fine_g_range)



def main():
    pass
#    y,x  = svm_read_problem("D:/张知临/源代码/python_ctm/model/im_info/temp/svm.train")
#    fw= file("D:/张知临/源代码/python_ctm/model/im_info/temp/svm.param",'w')
#    c,g=grid_search_for_large_data(y,x,fw)  
#    fw.close()
#    print c,g
#    y,x= svm_read_problem("D:/张知临/源代码/python_ctm/model/im_info/temp/svm.train")
#    sub_y,sub_x=subset(y,x,50)
#    save_dic_train_sample(file("D:/sub.txt",'w'),sub_y,sub_x)

def subset(y,x,sub_num):
    sub_y=[]
    sub_x=[]
    class Label:
        def __init__(self, label, index, selected):
            self.label = label
            self.index = index
            self.selected = selected

    # get labels
    i = 0
    labels = []
    for la in y:
        labels.append(Label(float(la), i, 0))
        i = i + 1
    l = i
       
    # select the subset

    labels.sort(key = lambda x: x.label)
    
    label_end = labels[l-1].label + 1
    labels.append(Label(label_end, l, 0))

    begin = 0
    label = labels[begin].label
    for i in range(l+1):
        new_label = labels[i].label
        if new_label != label:
            nr_class = i - begin
            k = i*sub_num//l - begin*sub_num//l
            # at least one instance per class
            if k == 0:
                k = 1
                
            for j in range(nr_class):
                if randint(0, nr_class-j-1) < k:
                    labels[begin+j].selected = 1
                    k = k - 1
            begin = i
            label = new_label
    for  i in range(len(y)):
        if labels[i].selected == 1:
            sub_y+=[y[i]]
            sub_x+=[x[i]]
    return sub_y,sub_x

#def save_dic_train_sample(f,y,x):
#    '''将构造的svm问题格式进行保存
#    y为list x为list里面为 词典。[ {} ]
#    '''
#    for i in range(len(y)):
#        f.write(str(y[i]))
#        #将字典有序的输出来。
#        #sorted(dic.items(),key=lamda dic:dic[0],reverse = False)
#        #dic =x[0]
#        #sorted_keys=sorted(dic.items(),key=lambda dic:dic[0],reverse=False)
#        sorted_keys = x[i].keys()
#        sorted_keys.sort()
#        for key  in sorted_keys:
#            f.write("\t"+str(key)+":"+str(x[i][key]))
#        f.write("\n")
            
    
    
