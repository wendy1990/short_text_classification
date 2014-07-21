#!/bin/bash
#该脚本统计PrepareServer,TextClassificationServer以及PageMarkServer的内存使用情况
#author: max.zhang
#date:   2013-12-20
#usage:  sh check_sum.sh

prepare_mem=`ssh H13.CLO.7FB15.NJDX.FMKJ.NET "ps xuf | grep 'java .*PageMarkServer' | grep -v grep | awk '{print $5"\t"$6}'"`
classify_mem=`ssh H14.CLO.7FB15.NJDX.FMKJ.NET "ps xuf | grep 'java .*PageMarkServer' | grep -v grep | awk '{print $5"\t"$6}'"`
pagemark_mem=`ssh H15.CLO.7FB15.NJDX.FMKJ.NET "ps xuf | grep 'java .*PageMarkServer' | grep -v grep | awk '{print $5"\t"$6}'"`
