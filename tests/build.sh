#!/bin/bash

set -e


echo "Building ZMK External Power Hold Module Test..."
ZEPHYR_BASE=$(west config 'zephyr.base')

if [[ -n $ZEPHYR_BASE ]]
then
    echo "found ZEPHYR_BASE: $ZEPHYR_BASE"

    Zephyr_DIR=$ZEPHYR_BASE/share/zephyr-package/cmake/

    LIB_BASE_DIR="$(dirname "$ZEPHYR_BASE")"

    ZMK_SRC_DIR=${LIB_BASE_DIR}/zmk/app

    export LIB_BASE_DIR
    export ZMK_SRC_DIR
    export Zephyr_DIR
else
    echo "found ZEPHYR_BASE not found: $ZEPHYR_BASE"
    echo "please run west config zephyr.base $(pwd)/zephyr"
fi



# Setup west workspace if not exists
if [ ! -d .west ]; then
    echo "Initializing west workspace..."
    west init -l ./config
    west update
fi

# Build the test application
echo "Building test application..."
west build -s zmk/app -d $(pwd)/.build/sofle_left -b nice_nano_v2 \
     -- -DZMK_CONFIG=$(pwd)/config -DZMK_EXTRA_MODULES=$(pwd) -DSHIELD=sofle_left 


cp .build/sofle_left/compile_commands.json ./
