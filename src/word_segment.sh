#!/bin/sh
#export LD_LIBRARY_PATH=~/bigben/external/boost/lib/:~/bigben/src/tokenizer/src/
~/bigben/src/tokenizer/test/test_tokenizer/tokenizer -i $1 -c 0 -r ~/bigben/src/tokenizer/external/ictclas50 -f 1 -o $2
