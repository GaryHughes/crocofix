FROM ubuntu:20.04

# This prevents a prompt regarding timezones.
ENV DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

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
    libbz2-dev \
    vim \
    python3 \
    libtinfo5 \
    systemtap-sdt-dev \
    bpftrace \
    liblua5.4-dev
   
#
# Clang
#
RUN curl -SL https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz | tar --strip-components 1 -xJC /usr

#
# Boost
#
RUN curl -SL https://sourceforge.net/projects/boost/files/boost/1.72.0/boost_1_72_0.tar.gz/download | tar -zxf - && \
    cd boost_1_72_0 && \
    ./bootstrap.sh --with-toolset=clang --prefix=/usr/local && \
    ./b2 toolset=clang cxxflags="-std=c++17" install
