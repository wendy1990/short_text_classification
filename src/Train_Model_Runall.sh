#!/bin/sh
sh pre_process.sh $1 ../sample/post.train.clear
var_path=../data/model/
echo $var_path
var_model=${var_path}$2
echo $var_model
export LD_LIBRARY_PATH=~/bigben/external/boost/lib/:~/bigben/src/tokenizer/src/:~/bigben/external/jsoncpp/lib
sh word_segment.sh ../sample/post.train.clear ../data/temp/post.train.segmented
sh feature_selection.sh ../data/temp/post.train.segmented_0.txt
sh cons_trainsample_for_libsvm.sh ../data/temp/post.train.segmented_0.txt ../data/temp/svm.train
sh svm_c_g_param.sh ../data/temp/svm.train ../data/temp/svm.param
sh train_svm.sh  ../data/temp/svm.train $var_model
echo "训练模型成功！"
