#!/bin/sh
#完成log4cplus的下载，编译，安装
#usage: sh build_log4cplus.sh install_path

prefix=$1
wget http://nchc.dl.sourceforge.net/project/log4cplus/log4cplus-stable/1.0.4/log4cplus-1.0.4.1.tar.gz
tar zxf log4cplus-1.0.4.1.tar.gz
cd log4cplus-1.0.4.1
./configure --prefix=${prefix} CXXFLAGS="-g -O2"
make -j8
make install
cd -
echo "build log4cplus over!"

rm log4cplus-1.0.4.1.tar.gz -f
rm log4cplus-1.0.4.1 -rf
