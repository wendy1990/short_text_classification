#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author:张知临 zhzhl202@163.com
#Filename: stem.py

import porter2

def stemFile(filename,str_splitTag,tc_splitTag):
    tempFilename="../STEM.TEMP"
    tempFile =  file(tempFilename,'w')
    for line in file(filename):
       tempFile.write(tc_splitTag.join([stemLine(words,str_splitTag) for  words in line.strip().split(tc_splitTag)]))
       tempFile.write("\n")
    tempFile.close()
    f = file(filename,'w')
    for line in file(tempFilename):
        f.write(line)
    f.close()    
        
def stemWords(word):
    return porter2.stem(word).lower()

def stemLine(words,str_splitTag):
    return str_splitTag.join([stemWords(word) for word in words.strip().split(str_splitTag)])

if __name__=="__main__":
    stemFile("../test.txt","^","\t")
        
    