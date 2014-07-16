#!/bin/bash
#该脚本用于训练模型前，对训练样本进行降噪处理
#usage:
#	sh pre_process.sh sample output-file
#		sample: 样本文件
#		output-file:降噪后的文件

if test -f $1
then
	#去掉时间
    sed  "s/\([0-9]\{0,4\}年\)\?[0-9]\{0,2\}月[0-9]\{0,2\}日/ /g" $1>$2 
	#去掉url
	sed -i "s/\(\(\(http\|ftp\|https\)\:\/\/\)\|www\)[0-9,a-z,A-Z,\/,\.]*//g" $2
	#去掉样本中的(央视记者)这种噪音
	sed -i "s/（.\{0,11\}）//g" $2
	sed -i "s/(.\{0,11\})//g" $2
	#去掉表情符号,会有少量误删除
	sed -i "s/\[.\{1,3\}\?\]//g" $2
	#去掉未过滤掉的标签 &gt;
	sed -i "s/&\w*;\?//g" $2
	#去掉微博中位置信息
	sed -i "s/我在这里:[^\.]*//g" $2
	sed -i "s/我在[:,：]\w*/ /g" $2

    sed -i "s/[(,（]\w*图\w*[),）]/ /g" $2 
    sed -i "s/图自[:,：]\?\w*/ /g" $2
    sed -i "s/组图[:,：]\?/ /g" $2
    sed -i "s/分享来\?自@\w*/ /g" $2 
    sed -i "s/@\w*/ /g" $2
    sed -i "s/#\w*#/ /g" $2
    sed -i "s/[0-9]\{1,4\}[年,月,时,分]/ /g" $2
	#删除掉降噪后为空的行
	sed -i "/^1	$/d" $2
	sed -i "/^-	$/d" $2
fi