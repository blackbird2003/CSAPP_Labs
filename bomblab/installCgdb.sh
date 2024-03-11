#! /bin/bash
# You must have the following packages installed.
    # sh
    # autoconf
    # automake
    # aclocal
    # autoheader
    # libtool
    # flex
    # bison
    # gcc/g++ (c11/c++11 support)
cd /tmp
! ( test -d cgdb) && git clone https://github.com/cgdb/cgdb.git  
# 下面是源镜像 https://github.com/cgdb/cgdb.git
sudo apt-get update
sudo apt install automake -y
sudo apt-get install flex -y
sudo apt-get install texinfo -y
sudo apt-get install libreadline-dev -y

cd cgdb
./autogen.sh
cd ..
build="qweasd13sdfx12esdui1_" # 随机文件名
mkdir $build
cd $build
../cgdb/configure --prefix=$PWD/../prefix
make -srj10
make install

cd cgdb
( test -e  /usr/local/bin/cgdb) && echo "rm -rf /usr/local/bin/cgdb" && sudo rm -rf  /usr/local/bin/cgdb
sudo mv ./cgdb  /usr/local/bin/
message="\n----------------  cgdb install error ---------------- " 
(test -e /usr/local/bin/cgdb ) && message="\n---------------- cgdb install successful     -----------" 
echo -e $message
cd /tmp
( test -d $build) && rm -rf $build

# 这里大概设置了搜索大小写不敏感，tab 大小，分屏方式（默认改为左右分屏）：
echo -e "set ignorecase\nset ts=4\nset wso=vertical\nset eld=shortarrow\nset hls\n" > ~/.cgdb/cgdbrc
