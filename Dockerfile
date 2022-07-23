FROM ubuntu:22.04

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
    liblua5.4-dev \
    clang-tidy
   
#
# Clang
#
RUN curl -SL https://github.com/llvm/llvm-project/releases/download/llvmorg-14.0.0/clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz | tar --strip-components 1 -xJC /usr

#
# Boost
#
RUN curl -SL https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz | tar -zxf - && \
    cd boost_1_79_0 && \
    ./bootstrap.sh --with-toolset=clang --prefix=/usr/local && \
    ./b2 toolset=clang cxxflags="-std=c++17" install
