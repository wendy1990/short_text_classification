#!/usr/bin/python
#-*-coding:utf8-*-
'''
Created on 2013-12-22
@author: zyy_max
@brief: build feature vector with word_dict and token_list as hadoop streaming
'''
import sys

word_dict = {}
with open('word.dict', 'r') as ins:
    for line in ins:
        l = line.split()
        try:
            word_dict[l[1].decode('utf8')] = int(l[0])
        except:
            print l[1]
def compute(token_list):
    feature = [0]*len(word_dict)
    for token in token_list:
        if not isinstance(token, unicode):
            token = token.decode('utf8')
        feature[word_dict[token]] += 1
    feature_nonzero = [(idx,value) for idx, value in enumerate(feature) if value > 0]
    return feature_nonzero

for line in sys.stdin:
    oriid,_,line = line.partition('\t')
    feat = compute(line.strip().split())
    print '%s\t%s' % (oriid, ' '.join(['%s:%s' %(idx,value) for idx, value in feat]))
