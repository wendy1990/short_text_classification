#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author: 张知临 zhzhl202@163.com
import tms,os
#tms.tms_train("../data/weijin.train",indexes=[4],main_save_path="../model/",stopword_filename="../data/stopwords.txt",svm_type="liblinear",config_name="linear_title.config",dic_name="linear_title.key",model_name="linear_title.model",train_name="linear_title.train",param_name="linear_title.param",ratio=0.4,seg=1,local_fun="tf",global_fun="rf")
#tms.tms_predict_multi("../data/weijin.test",config_files=["../model/model/linear_title.config"],indexes_lists=[[4]],result_save_path="../result/linear_title.result",result_indexes=[0],seg=1)
#tms.tms_analysis("../result/linear_title.result",step=4,output_file="../data/linear_title.analysis",indexes=[0,1,2],predicted_label_index=0,predicted_value_index=1,true_label_index=2,min=0,max=2)=======
#import tms
#tms.tms_predict_multi("../data/weijin.test", ["../data/aliws/model/lineartitle.config","../data/aliws/model/lineartitle_content.config","../data/aliws/model/svmtitle.config","../data/aliws/model/svmtitle_content.config"],[[2],[2,3],[2],[2,3]],result_indexes=[0,1,2,3,4],result_save_path="../data/weijin.result")>>>>>>> .r167


#先根据训练样本取得词典后，然后再根据词典把测试样本转换为相应的格式。
filepath = u"E:\\算法与技术学习\\y语料库\\PaperCopus\\"
trainFile=u"reuters_raw_train_0.txt"
testFile=u"reuters_raw_test_0.txt"
tms.tms_train(os.path.join(filepath,trainFile),main_save_path=filepath,stopword_filename=u"E:\\算法与技术学习\\y语料库\\PaperCopus\\stopwords.txt",svm_type="liblinear",config_name="weijin_ik_20120110.config",dic_name="weijin_ik_20120110.key",model_name="weijin_ik_20120110.model",train_name="weijin_ik_20120110.train",param_name="weijin_ik_20120110.param",ratio=0.4,seg=0,local_fun="tf",global_fun="rf",str_splitTag=" ",tc_splitTag="\t")
tms.cons_train_sample_for_svm(os.path.join(filepath,testFile), u"E:\\算法与技术学习\\y语料库\\PaperCopus\\model\\weijin_ik_20120110.key",u"E:\\算法与技术学习\\y语料库\\PaperCopus\\new.txt", local_fun="tf",str_splitTag=" ",tc_splitTag="\t")