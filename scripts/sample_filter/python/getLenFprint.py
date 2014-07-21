#!/usr/bin/env python
#-*-coding:utf8-*-
row_dict = {}
with open('short_uniq_60_pos.row') as ins:
    for line in ins:
        id,lenth,score = line.strip().split('\t')
        row_dict[id] = [lenth,score]

with open('short_uniq_60_fprint/part-00000') as ins:
    for line in ins:
        id,_,fprint = line.strip().partition('\t')
        if id in row_dict:
            row_dict[id].append(fprint)

outs = open('short_uniq_60_pos.lenfprint', 'w')
for key, value in row_dict.items():
    if len(value) < 3:
            continue
    outs.write('%s\t%s\t%s\t%s\n' %(key,value[0],value[1],value[2]))
