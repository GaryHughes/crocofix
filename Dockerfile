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
    # begin llvm.sh requirements
    lsb-release \
    wget \
    software-properties-common \
    gnupg \
    # end llvm.sh requirements
    # begin opentelemetry-cpp requirements 
    libcurl4-openssl-dev \
    libprotobuf-dev \
    protobuf-compiler
    # end opentelemetry-cpp requirements
   
#
# Clang
#
RUN curl -o llvm.sh https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    yes | ./llvm.sh 15 all && \
    ln -sf /usr/lib/llvm-15/bin/clang /usr/bin && \
    ln -sf /usr/lib/llvm-15/bin/clang++ /usr/bin && \
    ln -sf /usr/lib/llvm-15/bin/clang-tidy /usr/bin


#
# Boost
#
RUN curl -SL https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.gz | tar -zxf - && \
    cd boost_1_80_0 && \
    ./bootstrap.sh --with-toolset=clang --prefix=/usr/local && \
    ./b2 toolset=clang cxxflags="-std=c++17" install

#
# opentelemetry-cpp
#
RUN git clone --recursive https://github.com/open-telemetry/opentelemetry-cpp && \
    cd opentelemetry-cpp && \
    mkdir build && cd build && \
    cmake -DBUILD_TESTING=OFF -DWITH_OTLP=ON .. && \
    cmake --build . && \
    cmake --install .