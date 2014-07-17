
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


def save_train_for_lsa(test_path,model_save_path,lsa_train_save_path):
    '''predict trainset using the initial classifier  ,and save the trainset with
    lsa format : label score feature
    '''
    y,x = svm_read_problem(test_path)
    m = svm_load_model(model_save_path)
    p_lab,p_acc,p_sc = svm_predict(y,x,m)
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
    ctm_lsa(M,threshold,K,for_lsa_train_save_path,lsa_train_save_path,lsa_save_path)
    
    print"--------------------选择最优的c,g------------------------------"
    search_result_save_path  = main_save_path +"temp/"+param_name
    c,g=grid(lsa_train_save_path,search_result_save_path)
    
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
