#!/bin/sh
#该脚本完成mahout的安装

mkdir -p /home/${USER}/KeywordExtract/script/mahout
wget http://apache.fayea.com/apache-mirror/mahout/0.8/mahout-distribution-0.8.tar.gz
tar zxf mahout-distribution-0.8.tar.gz -C /home/${USER}/KeywordExtract/script/mahout

export MAHOUT_HOME=/home/${USER}/KeywordExtract/script/mahout/mahout-distribution-0.8
export PATH=$MAHOUT_HOME/bin:$PATH

echo "build mahout 0.8 over!"
exit 0
