#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: example.py

import tms
'''采用程序默认参数对模型训练、预测、结果分析'''
#模型训练，输入的文件为binary_seged.train,需要训练的为文件中的第1个字段(第0个字段为lablel),保存在data文件夹中。特征选择保留top %10的词，使用liblinear
#tms.tms_train("../data/binary_seged.train")
#模型预测，
#tms.tms_predict("../data/binary_seged.test","../data/model/tms.config",result_save_path="../data/binary_seged.result")
#对结果进行分析
#tms.tms_analysis("../data/binary_seged.result")

'''配置多个模型进行预测'''
#tms.tms_predict_multi("../data/binary_seged.test", ["../data/libsvm_model/tms.config","../data/liblinear_model/tms.config"],indexes_lists=[[1],[1]],result_save_path="../data/binary_seged.result")
#tms.tms_analysis("../data/binary_seged.result",indexes=[0,1,2,3,4],true_label_index=4)

'''对文件进行分词'''
#tms.tms_segment("../data/binary.train", indexes=[1])

'''特征选择'''
#tms.tms_feature_select("../data/binary_seged.train", indexes=[1], global_fun="idf", dic_name="test.key", ratio=0.05, stopword_filename="")

'''将输入文件构造为libsvm和liblinear的输入格式'''
#tms.cons_train_sample_for_svm("../data/binary_seged.train", "../data/model/dic.key", "../data/tms.train", [1])

'''对SVM模型选择最优的参数'''


'''对没有经过分词的文件进行训练'''
#tms.tms_train("../data/binary.train",seg=1)

'''假设data文件夹下有一个post.train和post.test的训练样本和测试样本，每一行有3个字段：label title content。样本都没有分词
该例子需要完成：
1、对title进行分词、训练，模型保存在../data/post/ 下，所有的文件都有title命名，SVM模型选择使用libsvm，核函数使用rbf,选择选择保留top 40%的词，特征权重使用tf*idf
2、对title和content一起进行分词、训练，模型保存在../data/post/ 下，所有的文件都有title_content命名，SVM模型选择使用liblinear，选择选择保留top 20%的词，特征权重使用tf
3、先对post.test进行分词，然后使用已经训练好的模型对post.test进行预测。结果以post.result命名，将原label与结果一同输出。
4、计算模型的预测F值、Recall、Precision，并将结果输出在屏幕上。
5、计算从[0,1]区间内各个阈值下对应的F值、Recall、Precision，将结果保存在post.analysis
'''
tms.tms_train("../data/post.train",indexes=[1],main_save_path="../data/",stopword_filename="../data/stopwords.txt",svm_type="libsvm",svm_param="-t 2",config_name="title.config",dic_name="title.key",model_name="title.model",train_name="title.train",param_name="title.param",ratio=0.4,seg=1,local_fun="tf",global_fun="idf")
#tms.tms_train("../data/post.train",indexes=[1,2],main_save_path="../data/",stopword_filename="../data/stopwords.txt",svm_type="liblinear",config_name="title_content.config",dic_name="title_content.key",model_name="title_content.model",train_name="title_content.train",param_name="title_content.param",ratio=0.2,seg=1,local_fun="tf",global_fun="one")
#tms.tms_predict_multi("../data/post.test",config_files=["../data/model/title.config","../data/model/title_content.config"],indexes_lists=[[1],[1,2]],result_save_path="../data/post.result",result_indexes=[0],seg=1)
#tms.tms_analysis("../data/post.result",step=2,output_file="",indexes=[0,1,2],predicted_label_index=0,predicted_value_index=1,true_label_index=2)
#tms.tms_analysis("../data/post.result",step=4,output_file="../data/post.analysis",min=0,max=1,indexes=[0,1,2],predicted_label_index=0,predicted_value_index=1,true_label_index=2)