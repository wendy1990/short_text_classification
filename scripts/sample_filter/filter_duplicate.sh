#!/bin/bash
#该脚本对文本进行必要的处理(去噪,分词,生成特征字典,提取指纹)，再进行自去重
#author:  max.zhang
#date:    2013-12-25
#usage:   sh filter_duplicate.sh data/22-24_short.content

#echo "Filtering duplicate in $1"
dir_path=`echo $1 | awk '{split($0, arr, "/");print arr[1];}'`
prefix=`echo $1 | awk '{split($0, arr, "/");print arr[2];}' | awk -F '.' '{print $1}'`
full_prefix=$dir_path/$prefix

#过滤字符，提取有效文本
sh webcontent_filter.sh $1 "$full_prefix.ori"
cat "$full_prefix.ori" | awk '{if(length($0)>60){print NR"\t"$0}}' > "$full_prefix.rowori"
rm -f "$full_prefix.ori"

#分词
python python/tokens.py '-s'  "$full_prefix.rowori" "$full_prefix.token" 'c' 'data/stopwords.txt'
rm -f "$full_prefix.rowori"

#生成特征词典
python python/DictBuilder.py "$full_prefix.token" "$full_prefix.dict"

#用hadoop streaming生成Simhash指纹
cp "$full_prefix.dict" 'word.dict'
hadoop fs -rm "/test/max.zhang/sample_data/$prefix.token"
hadoop fs -put "$full_prefix.token" '/test/max.zhang/sample_data'
hadoop fs -rmr "/test/max.zhang/sample_data/"$prefix"_rowfprint"
hadoop jar /home/app/srv/hadoop/contrib/streaming/hadoop-streaming-1.2.1.jar -mapper python/SimhashMapper.py -file python/SimhashMapper.py -input "/test/max.zhang/sample_data/$prefix.token" -output "/test/max.zhang/sample_data/"$prefix"_rowfprint" -file 'word.dict'
rm -f 'word.dict'
rm -rf "$full_prefix"_rowfprint
hadoop fs -get "/test/max.zhang/sample_data/"$prefix"_rowfprint" $dir_path

#由.token和rowfprint/part-00000生成.lenfprint
sort -k1 -n "$full_prefix.token" > tmp_1
sort -k1 -n ""$full_prefix"_rowfprint/part-00000" > tmp_2
paste tmp_1 tmp_2 |
awk -F '\t' '{print $1"\t"length($2)"\t"$4}' > $full_prefix.lenfprint
rm -f tmp_1
rm -f tmp_2
#自去重
python python/FilterSalientBySimhashNoScore.py $full_prefix.lenfprint \
     $full_prefix.lenfprint "$full_prefix"_uniq.lenfprint
