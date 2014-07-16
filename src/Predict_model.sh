#!/bin/sh
#sh pre_process.sh $1 ../sample/post.test.clear
#export LD_LIBRARY_PATH=~/bigben/external/boost/lib/:~/bigben/src/tokenizer/src/
#sh word_segment.sh ../sample/post.test.clear ../data/temp/post.test.segmented
#sed -i "s/- 1/-1/g" ../data/temp/post.test.segmented_0.txt
sh cons_trainsample_for_libsvm.sh ../data/temp/post.test.segmented_0.txt ../data/temp/svm.test
sh predict_post.sh ../data/temp/svm.test $2 
sh ananlysis.sh $2 $3
echo "模型预测完成！"                     
