#!/usr/bin/env python
#-*-coding:utf8-*-
"""
    scripts:	get_row_sample.py
    brief:	根据*.content和*_uniq.lenfprint提取新的有效样本
    author:	max.zhang
    date:	2013-12-25
    usage:	python get_row_sample.py short.content short_uniq.rowfprint
"""
import os
import sys

if len(sys.argv) < 3:
    print 'Usage:\tpython get_row_sample.py short.content short_uniq.rowfprint'
    sys.exit(1)
row_dict={}
with open(sys.argv[2]) as ins:
    for line in ins:
        row = line.strip().split('\t')[0]
        row_dict[row] = 0
with open(sys.argv[1]) as ins:
    for lineid, line in enumerate(ins):
        row = str(lineid+1)
        if not row in row_dict:
            continue
        print line.strip()
