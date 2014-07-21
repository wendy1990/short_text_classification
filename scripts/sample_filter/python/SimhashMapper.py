#!/usr/bin/env python
# -*- coding=utf-8 -*-
'''
Created on 2013-10-13
@author: zyy_max
@brief: build simhash and compute hamming_distance
@modified: 2013-10-15 ==> add update_word for SimhashBuilder
'''

# Implementation of Charikar simhashes in Python
# See: http://dsrg.mff.cuni.cz/~holub/sw/shash/#a1

import os, sys
import time

hashval_dict = {}

def _string_hash(word, hashbits=128):
    # A variable-length version of Python's builtin hash
    if word == "":
        return 0
    else:
        x = ord(word[0])<<7
        m = 1000003
        mask = 2**hashbits-1
        for c in word:
            x = ((x*m)^ord(c)) & mask
        x ^= len(word)
        if x == -1:
            x = -2
        return x

def sim_hash_token(token_list, hashbits=128):
    finger_vec = [0]*hashbits
    for idx, token in enumerate(token_list):
        bitmask = 0
        #hashval = hashval_dict[token]
        hashval = _string_hash(token)
        for i in range(hashbits):
            bitmask = 1<<i
            if bitmask&hashval != 0:
                finger_vec[i] += 1
            else:
                finger_vec[i] -= 1
        #print finger_vec
    fingerprint = 0
    for i in range(hashbits):
        if finger_vec[i] >= 0:
            fingerprint += 1 << i
#整个文档的fingerprint为最终各个位大于等于0的位的和
    return fingerprint

'''
with open('word.dict', 'r') as ins:
    for line in ins:
        word = line.strip().split()[1]
        hashval_dict[word] = _string_hash(word)
'''

for line in sys.stdin:
    oriid,_,line = line.partition('\t')
    token_list = line.strip().split()
    fingerprint = sim_hash_token(token_list)
    print oriid+'\t'+str(fingerprint)


