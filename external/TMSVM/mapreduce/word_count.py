#!/usr/bin/env python
import sys
import os
from hstream import *
sys.path.append('.')
import tms_svm

def read_param(filename):
	params=list()
	for line in file(filename):
		params.append(line.strip())
	return params

default_param_file="./params"
params = read_param(default_param_file)

class WordCount(HStream):
    '''word count example for Hadoop Streaming using hstream'''
    def mapper(self,key, value):
        '''mapper function'''
	for param in params:
        	self.write_output( param, value )

    def reducer(self, key, values):
        '''reducer function'''
        length =  sum([1 for value in values])
       
        self.write_output( key, str(length))

if __name__ == '__main__':
    WordCount()
