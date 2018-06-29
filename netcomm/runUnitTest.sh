#!/bin/bash

export LD_LIBRARY_PATH=../utils/log/output:../utils/comm/output

killall -9 unittest
./unittest
