#!/usr/bin/env python
#-*-coding:utf8-*-
'''
Created on 2013-12-23
@author zyy_max
@brief 通过计算两两Simhash值的汉明距离，过滤掉重复文本
'''
import os
import sys
import math
import time
from Utils import hamming_distance

class DocFeatLoader:
    def __init__(self, docid, fprint, content_lenth):
        self.docid = docid
        self.lenth = content_lenth
        self.feat_vec = int(fprint)

def docset_similar(docset1, docset2, dist_row_file):
    # Test Euclidean Distance
    begin = time.time()    
    e_sum = 0
    elimit_dict1 = {}
    elimit_dict2 = {}
    if docset1 is docset2:
        elimit_dict2 = elimit_dict1
    begin = time.time()
    outs = open(dist_row_file, 'w')
    for idx1, doc1 in enumerate(docset1):
        if idx1 in elimit_dict1:
            continue
        for idx2 in xrange(idx1+1, len(docset2)):
            doc2 = docset2[idx2]
            if idx2 in elimit_dict2:
                continue
            hdist = hamming_distance(doc1.feat_vec, doc2.feat_vec)
            if hdist > 25:
                continue
            if doc1.lenth > doc2.lenth:
                elimit_dict2[idx2] = 0
            else:
                elimit_dict1[idx1] = 0
                break
        print 'Doc%s finished\tavg:%ss\r' % (idx1,(time.time()-begin)/(idx1+1)),
        if not idx1 in elimit_dict1:
            outs.write('%s\t%s\t%s\n' %(doc1.docid,doc1.lenth,doc1.feat_vec))
    #row_results = []
    #for idx, doc in enumerate(docset1):
    #    if idx not in elimit_dict:
    #        row_results.append(doc.docid+'\n')
    #with open(dist_row_file, 'w') as outs:
    #    outs.writelines(row_results)
        
def featset_similar(featset1, featset2, row_file):
    docset1 = []
    with open(featset1, 'r') as ins:
        for line in ins.readlines():
            docid, lenth, feat_str = line.strip().split('\t')
            docset1.append(DocFeatLoader(docid, feat_str, lenth))
    if os.path.realpath(featset1) == os.path.realpath(featset2):
        docset2 = docset1
    else:
        docset2 = []
        with open(featset2, 'r') as ins:
            for line in ins.readlines():
                docid, lenth, feat_str = line.strip().split('\t')
                docset2.append(DocFeatLoader(docid, feat_str, lenth))
    docset_similar(docset1, docset2, row_file)
    #with open(row_file, 'w') as outs:
    #    for doc in filterFunc(docset1):
    #        outs.write('%s\n' % doc.docid)
   
if __name__=="__main__":
    if len(sys.argv) < 3:
        print "Usage:\t",sys.argv[0]," <featset1> <featset2> <dist_row_file>"
        print "\t\tfeatset: id\tlength\tfeat"
        exit(-1)
    print 'Matching docset:%s and docset:%s...' %(sys.argv[1], sys.argv[2])
    featset_similar(sys.argv[1], sys.argv[2], sys.argv[3])
