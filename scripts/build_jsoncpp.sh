#!/bin/sh
#该脚本完成jsconcpp0.5.0的下载，编译，安装
#usage:sh build_jsoncpp.sh install_path

prefix=$1

wget http://nchc.dl.sourceforge.net/project/jsoncpp/jsoncpp/0.5.0/jsoncpp-src-0.5.0.tar.gz
tar zxf jsoncpp-src-0.5.0.tar.gz
cd jsoncpp-src-0.5.0/

wget http://garr.dl.sourceforge.net/project/scons/scons/2.3.0/scons-2.3.0.tar.gz
tar zxf scons-2.3.0.tar.gz  
MYSCONS=`pwd`
python $MYSCONS/scons-2.3.0/script/scons platform=linux-gcc  

mkdir -p $prefix/include
mkdir -p $prefix/lib

cp -rf $MYSCONS/include/json/ $prefix/include
cp -rf $MYSCONS/libs/linux*/*   $prefix/lib
ln -s $prefix/lib/libjson_linux-gcc-4.6_libmt.a  $prefix/lib/libjsoncpp.a
ln -s $prefix/lib/libjson_linux-gcc-4.6_libmt.so $prefix/lib/libjsoncpp.so

cd ..
rm -rf jsoncpp-src-0.5.0
rm jsoncpp-src-0.5.0.tar.gz

echo build jsoncpp over!
