#!/bin/sh
#该脚本完成boost_1_49_0的下载，编译，安装
#usage:sh build_boost.sh install_path

prefix=$1

#install libicu
#sudo yum install libicu-devel-3.6-5.11.4.x86_64
###sudo apt-get install libicu-dev
#make boost
#wget http://nchc.dl.sourceforge.net/project/boost/boost/1.49.0/boost_1_49_0.tar.gz
tar zxf boost_1_49_0.tar.gz
cd boost_1_49_0/
sh bootstrap.sh --prefix=${prefix} --with-icu=/usr/lib --with-libraries=date_time,regex,thread,program_options
./bjam install
echo "build boost over!"
cd -
#rm boost_1_49_0.tar.gz -f
rm boost_1_49_0 -rf
