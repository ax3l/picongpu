FROM       nvidia/cuda:7.0-devel
MAINTAINER Axel Huebl <a.huebl@hzdr.de>

ENV        HOME /root
ENV        DEBIAN_FRONTEND noninteractive

# install dependencies provided by packages
RUN        apt-get update \
           && apt-get install -y --no-install-recommends \
              autoconf \
              ca-certificates \
              git \
              libhdf5-openmpi-dev \
              libopenmpi-dev \
              libpng-dev \
              make \
              openssl \
              pkg-config \
              rsync \
              wget \
              zlib1g-dev \
           && rm -rf /var/lib/apt/lists/*

RUN        mkdir build

# CMake 3.3+
ENV CMAKE_DOWNLOAD_SUM d251b95829d29e4529dfdbc1e676cd0ef86e9544976603db2f5c26364ad71146
RUN        wget -nv https://cmake.org/files/v3.3/cmake-3.3.0-Linux-x86_64.tar.gz \
           && echo "$CMAKE_DOWNLOAD_SUM cmake-3.3.0-Linux-x86_64.tar.gz" | sha256sum -c --strict - \
           && tar -xzf cmake-3.3.0-Linux-x86_64.tar.gz -C /usr/bin       --strip-components=2 cmake-3.3.0-Linux-x86_64/bin/cmake \
           && tar -xzf cmake-3.3.0-Linux-x86_64.tar.gz -C /usr/share     --strip-components=2 cmake-3.3.0-Linux-x86_64/share \
           && tar -xzf cmake-3.3.0-Linux-x86_64.tar.gz -C /usr/share/doc --strip-components=2 cmake-3.3.0-Linux-x86_64/doc \
           && tar -xzf cmake-3.3.0-Linux-x86_64.tar.gz -C /usr/share/man --strip-components=2 cmake-3.3.0-Linux-x86_64/man \
           && rm cmake-3.3.0-Linux-x86_64.tar.gz

# boost
RUN        wget -nv http://downloads.sourceforge.net/project/boost/boost/1.57.0/boost_1_57_0.tar.gz \
           && tar -xzf boost_1_57_0.tar.gz && rm boost_1_57_0.tar.gz \
           && cd boost_1_57_0 \
           && ./bootstrap.sh \
              --prefix=/usr \
              --with-libraries=filesystem,program_options,regex,system,thread,math \
           && ./b2 link=shared variant=release runtime-link=shared -j2 \
           && ./b2 install \
           && cd / \
           && rm -rf boost_1_57_0 \
           && rm -rf /usr/lib/libboost*.a \
           && cd /usr/include/boost \
           && rm -rf geometry graph interprocess multiprecision phoenix polygon proto python spirit units xpressive

# pngwriter
RUN        git clone --depth 50 --branch master \
              https://github.com/pngwriter/pngwriter.git \
           && cd build \
           && cmake -DCMAKE_INSTALL_PREFIX=/usr ../pngwriter \
           && make install \
           && cd / \
           && rm -rf build/* pngwriter

# libSplash
RUN        git clone --depth 50 --branch master \
              https://github.com/ComputationalRadiationPhysics/libSplash.git \
           && cd build \
           && cmake -DCMAKE_INSTALL_PREFIX=/usr ../libSplash \
           && make install \
           && cd / \
           && rm -rf build/* libSplash

# ADIOS
RUN        wget -nv http://users.nccs.gov/~pnorbert/adios-1.10.0.tar.gz \
           && tar -xzf adios-1.10.0.tar.gz \
           && rm adios-1.10.0.tar.gz \
           && cd adios-1.10.0 \
           && CFLAGS="-fPIC" ./configure --disable-static --enable-shared \
              --prefix=/usr --with-mpi --with-zlib=/usr --disable-fortran \
           && make && make install \
           && cd / \
           && rm -rf adios-1.10.0

# get PIConGPU sources
RUN        git clone --depth 50 --branch release-0.2.0 \
               https://github.com/ComputationalRadiationPhysics/picongpu.git

# build PIConGPU examples
#ENV PIC_COMPILE_SUITE_CMAKE "-DCMAKE_CXX_STANDARD=11 -DBOOST_RESULT_OF_USE_TR1 -DCUDA_NVCC_FLAGS='-std=c++11'"
RUN        ./picongpu/compile -j 2 -q -l ./picongpu/examples ./build \
           ; rm -rf build/*
#RUN        ./picongpu/compile ./picongpu/examples/ThermalTest ./build

RUN        ./picongpu/createParameterSet ./picongpu/examples/ThermalTest ./case1 \
           && cd build \
           && ../picongpu/configure ../case1 \
           && make install \
           && cd / \
           && rm -rf build/*
