#!/bin/bash

SPIKE_PATH=riscv-isa-sim
PK_PATH=riscv-pk
OPENOCD_PATH=riscv-openocd

BUILD_SPIKE="yes"
BUILD_PK="yes"
BUILD_OPENOCD="yes"

# TODO verify that riscv toolchain is installed, we won't build it manually
git submodule update --init --recursive

if [ -e $SPIKE_PATH/build/spike ]; then
    BUILD_SPIKE="no"
fi

if [ -e $PK_PATH/build/pk ]; then
    BUILD_PK="no"
fi

if [ -e $OPENOCD_PATH/build/src/openocd ]; then
    BUILD_OPENOCD="no"
fi

mkdir -p $SPIKE_PATH/build
mkdir -p $PK_PATH/build
mkdir -p $OPENOCD_PATH/build

# build isa simulator
if [ $BUILD_SPIKE == "yes" ]; then
    echo "INFO: building spike now"
    cd $SPIKE_PATH/build && ../configure && make -j`nproc` && cd ../..
else
    echo "INFO: Spike already built, skipping"
fi

# build proxy kernel
if [ $BUILD_PK == "yes" ]; then
    echo "INFO: building pk now"
    cd $PK_PATH/build && ../configure --host=riscv64-unknown-elf && make -j`nproc` && cd ../..
else
    echo "INFO: Proxy kernel already built, skipping"
fi

# build openocd
if [ $BUILD_OPENOCD == "yes" ]; then
    echo "INFO: building openocd now"
    cd $OPENOCD_PATH && git apply ../misc/0001-ignore-include.patch &&\
    ./bootstrap && cd build && ../configure --enable-remote-bitbang && make -j`nproc` && cd ../..
else
    echo "INFO: openocd already built, skipping"
fi
