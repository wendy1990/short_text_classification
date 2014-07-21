#!/bin/sh
#export LD_LIBRARY_PATH=~/2014/short_text_classification/external/boost/lib/:~/2014/short_text_classification/src/tokenizer/src/
~/2014/short_text_classification/src/tokenizer/test/test_tokenizer/tokenizer -i $1 -c 0 -r ~/2014/short_text_classification/src/tokenizer/external/ictclas50 -f 1 -o $2
