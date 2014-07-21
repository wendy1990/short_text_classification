#!/bin/bash
#该脚本用于均匀分割一个大文件
#usge: sh split_file.sh big-file part-num prefix
#	big-file: 要分割的大文件
#	part-num: 要分割成几份
#	prefix: 分割后小文件的前缀

usage()
{
	echo "usge: sh split_file.sh big-file part-num prefix"
	echo "	big-file: 要分割的大文件"
	echo "	part-num: 要分割成几份"
	echo "	prefix: 分割后小文件的前缀"
}

if [ $# -lt 3 ] ; then
	usage
	exit -1
fi

line_cnt=`wc -l $1|awk '{print $1}'`
if [ $line_cnt -le $2 ]; then
  echo "file [$1] line number less part-num. see usage"
  usage
  exit -1
fi

part_line_num=`expr $line_cnt / $2`
split -l $part_line_num $1 $3

