#!/bin/sh
#该脚本完成ICE的安装

prefix=$1

#wget http://www.zeroc.com/download/Ice/3.4/Ice-3.4.2.tar.gz
wget http://www.zeroc.com/download/Ice/3.5/Ice-3.5.0.tar.gz
tar zxf Ice-3.5.0.tar.gz
cd Ice-3.5.0/cpp
#rewrite ice config
sed -i 's/^prefix/#prefix/g' config/Make.rules
sed -i "/^#prefix/ a prefix ?= ${prefix}" config/Make.rules
sed -i "s/^embedded_runpath_prefix/#embedded_runpath_prefix/g" config/Make.rules
sed -i "s/^#OPTIMIZE/OPTIMIZE/g" config/Make.rules

make -j8
make install
cd -
#setup ice for python
cd Ice-3.5.0/py
sed -i 's/^prefix/#prefix/g' config/Make.rules
sed -i "/^#prefix/ a prefix ?= ${prefix}4py" config/Make.rules
sed -i "s/^embedded_runpath_prefix/#embedded_runpath_prefix/g" config/Make.rules
sed -i "s/^#OPTIMIZE/OPTIMIZE/g" config/Make.rules

export ICEPY_HOME=${prefix}4py
export PYTHONPATH=$ICEPY_HOME/python:$PYTHONPATH

make -j8
make install

rm Ice-3.5.0 -rf
echo "build Ice3.5.0 over!"
exit 0
