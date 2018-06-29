#!/bin/bash

## 当前绝对路径 ##
CUR_DIR_ABSLOTE=`pwd`

## 当前相对路径 ##
CUR_DIR_RELATIVE=`dirname $0`

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../${CUR_DIR_RELATIVE}/bin/

cd bin
./output/basicsrv