#!/bin/bash
#该脚本完成生产发布的编译环境打包
#usage: sh make_tar.sh /home/app/bigben /home/app/windbell/bigben
src_root=$1
dst_root=$2
bin_root=$dst_root/bin
conf_root=$dst_root/conf
long_tfidf_data=$dst_root/data/long_text_classification/tfidf
short_tfidf_data=$dst_root/data/short_text_classification/tfidf
ictclas_dict_data=$dst_root/data/dict/ictclas
log_root=$dst_root/logs
lib_root=$dst_root/libs
script_root=$dst_root/scripts
#创建目录
rm -rf $dst_root
mkdir -p $bin_root
mkdir -p $conf_root
mkdir -p $long_tfidf_data
mkdir -p $short_tfidf_data
mkdir -p $ictclas_dict_data
mkdir -p $log_root
mkdir -p $lib_root
mkdir -p $script_root

cur_dir=`pwd`
#Make
cd $src_root/src
echo "make projects..."
make

cd $cur_dir

#拷贝conf
echo "copy conf..."
cp $src_root/src/prepare/conf/* $conf_root
cp $src_root/src/prepare_proxy/conf/* $conf_root
cp $src_root/src/text_classification/classification_server/conf/* $conf_root
cp $src_root/src/page_mark/conf/* $conf_root
cp $src_root/src/text_classification/long_text_classification/conf/long_tfidf_predict.conf $conf_root
cp $src_root/src/text_classification/short_text_classification/src/short_TFIDF_predict/conf/* $conf_root

#拷贝lib
echo "copy libs..."
#ice
cp -a $src_root/external/ice/lib64/* $lib_root
#ice4py
cp -a $src_root/external/ice4py/python/IcePy.* $lib_root
#boost
cp -a $src_root/external/boost/lib/* $lib_root
#ICTCLAS
cp -a $src_root/external/ICTCLAS/*.a $lib_root
#jsoncpp
cp -a $src_root/external/jsoncpp/lib/* $lib_root
#kafka
cp -a $src_root/external/kafka/lib/* $lib_root
#libsvm
cp -a $src_root/external/libsvm/lib/* $lib_root
#log4cplus
cp -a $src_root/external/log4cplus/lib/* $lib_root
#PageMarkServer libs
cp -a $src_root/src/page_mark/libs/* $lib_root
#tokenzier libs
cp -a $src_root/src/tokenizer/src/libnlp_tokenizer.so $lib_root
#text classifiction
cp -a $src_root/src/text_classification/long_text_classification/train_model/predict_function/*.so $lib_root
cp -a $src_root/src/text_classification/short_text_classification/src/short_TFIDF_predict/*.so $lib_root

#拷贝data
echo "copy data..."
#long classification data
cp -a $src_root/src/text_classification/long_text_classification/data/model/* $long_tfidf_data
#short classification data
cp -a $src_root/src/text_classification/short_text_classification/data/model/* $short_tfidf_data
#ICTCLAS词典及配置
cp -a $src_root/src/tokenizer/external/ictclas50/* $ictclas_dict_data

#拷贝可执行程序
echo "copy bin..."
cp $src_root/src/prepare_proxy/prepare_proxy $bin_root
cp $src_root/src/prepare/prepare_svr $bin_root
cp $src_root/src/text_classification/classification_server/classification_svr $bin_root
cp $src_root/src/page_mark/target/PageMarkServer-1.0.jar $bin_root

#拷贝脚本文件
echo "copy scripts..."
cp $src_root/scripts/tools/check_restart.sh $script_root
cp $src_root/src/prepare_proxy/prepare_proxy_statistic_log_backup.sh $script_root
cp $src_root/src/prepare/prepare_statistic_log_backup.sh $script_root
cp $src_root/src/text_classification/classification_server/predict_res_extract.sh $script_root
cp $src_root/src/text_classification/classification_server/mail_with_attachfile_send.py $script_root
cp $src_root/src/text_classification/classification_server/mail_content.txt $script_root
cp $src_root/src/page_mark/scripts/statistic_pagemark_log.sh $script_root
cp -r $src_root/scripts/log_extract $script_root

#打包
echo "make pack..."
tar -zcf "$dst_root.tar.gz" $dst_root

