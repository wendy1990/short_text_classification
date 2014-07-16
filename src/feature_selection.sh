#!/bin/sh
python feature_selection.py -i $1 -d  dic.key -s ../conf/stopwords.txt -g idf
