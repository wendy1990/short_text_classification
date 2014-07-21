#!/bin/bash
#该脚本长短样本的过滤，长文本title+content以及短文本forward+content的抽取
#author: max.zhang
#date:   2013-12-25
#usage:  sh preprocess.sh 22-24.content

ori_content=$1
prefix=`echo "$ori_content" | awk -F '.' '{print $1}'`
short_content=""$prefix"_short.content"
long_content=""$prefix"_long.content"

#抽取长文本
cat $1 | awk -F '\t' '{if($2 !~ /[^a-z^A-Z]weibo[^a-z^A-Z]|[^n]t\.qq|[^ci]t\.sohu/)print $0}' |
awk -F '\t' '{print $3" "$4}' | sort -u >  $long_content

#抽取短文本
cat $1 | awk -F '\t' '{if($2 ~ /[^a-z^A-Z]weibo[^a-z^A-Z]|[^n]t\.qq|[^ci]t\.sohu/)print $0}' |
awk -F '\t' '{if(NF>10){print $6" "$11} else{print $5}}' | sort -u > $short_content

#执行去重脚本
#短文本去重
sh filter_duplicate.sh "$short_content" 
#长文本去重
#sh filter_duplicate.sh "$long_content" 
