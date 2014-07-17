#!/usr/bin/env python

import sys

sys.path.append('.')
import random
from hstream import *


def randomNM(n,m):
    a = random.randint(0,n)
    if a>0 and a<m:
        return True
    return False

N=35310000 
M=20000 

class Samping(HStream):

    def mapper(self,key,value):
        if randomNM(N,M) is True:
            print value
    
    
    def reducer(self,key,value):
        pass
    

if __name__== "__main__":
    Samping()
