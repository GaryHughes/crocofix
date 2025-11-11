FROM ubuntu:25.04

# This prevents a prompt regarding timezones.
ENV DEBIAN_FRONTEND=noninteractive
# Required to find boost shared libraries when running tests.
ENV LD_LIBRARY_PATH=/usr/local/lib

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
    python3-dev \
    systemtap-sdt-dev \
    bpftrace \
    liblua5.4-dev \
    catch2 \
    # begin llvm.sh requirements
    lsb-release \
    wget \
    software-properties-common \
    gnupg \
    # end llvm.sh requirements
    # begin vcpkg requirements
    zip \ 
    unzip \
    ninja-build \
    # end vcpkg requirements
    # pybind11 via vcpkg requirements
    autoconf-archive \
    automake
    # end pybind11 via vcpkg requirements

#
# vcpkg
#
RUN git clone https://github.com/microsoft/vcpkg.git \   
    && cd vcpkg \ 
    && ./bootstrap-vcpkg.sh
ENV VCPKG_ROOT=/vcpkg
ENV PATH=$VCPKG_ROOT:$PATH

#
# Clang
#
RUN curl -o llvm.sh https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    yes | ./llvm.sh 21 all && \
    ln -sf /usr/lib/llvm-21/bin/clang /usr/bin && \
    ln -sf /usr/lib/llvm-21/bin/clang++ /usr/bin && \
    ln -sf /usr/lib/llvm-21/bin/clang-tidy /usr/bin


#
# Boost
#
RUN curl -SL https://archives.boost.io/release/1.89.0/source/boost_1_89_0.tar.gz | tar -zxf - && \
    cd boost_1_89_0 && \
    ./bootstrap.sh --with-toolset=clang --prefix=/usr/local && \
    ./b2 toolset=clang cxxflags="-std=c++23" install