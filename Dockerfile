FROM ubuntu:18.04

#
# Prerequisites
#
RUN apt-get update && apt-get install -y \
    xz-utils \ 
    build-essential \ 
    curl \
    cmake \
    git \
    autoconf \
    libtool \
    zlib1g \
    zlib1g-dev \
    libbz2-dev
   
#
# Clang
#
RUN curl -SL https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz | tar -xJC . && \
    mv clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04 clang_10.0.0 && \
    echo 'export PATH=/clang_10.0.0/bin:$PATH' >> ~/.bashrc && \
    echo 'export LD_LIBRARY_PATH=/clang_10.0.0/lib:$LD_LIBRARY_PATH' >> ~/.bashrc

#
# Boost
#
RUN curl -SL https://sourceforge.net/projects/boost/files/boost/1.72.0/boost_1_72_0.tar.gz/download | tar -zxf - && \
    cd boost_1_72_0 && \
    . ~/.bashrc && \
    ./bootstrap.sh --with-toolset=clang --prefix=/usr/local && \
    ./b2 toolset=clang cxxflags=-std=c++17 install

