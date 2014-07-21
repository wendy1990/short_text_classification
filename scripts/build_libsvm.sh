#!/bin/sh
#该脚本完成libsvm-3.17的下载，编译，安装
#usage:sh build_libsvm.sh install_path


prefix=$1

wget http://www.csie.ntu.edu.tw/~cjlin/libsvm/libsvm-3.17.tar.gz
tar zxf libsvm-3.17.tar.gz

cd libsvm-3.17

#delete unuse files

rm svm-*.*

mkdir -p $prefix/include
mkdir -p $prefix/lib

#produce svm.so
g++ -fpic -shared -g -o libsvm.so svm.cpp -I ./

cp libsvm.so $prefix/lib
cp svm.h $prefix/include

cd ..

echo "build libsvm-3.17 over!"

rm libsvm-3.17.tar.gz -f
rm libsvm-3.17 -rf
