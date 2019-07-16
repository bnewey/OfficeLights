#!/bin/bash

g++ -g /home/bbnewey/visual-workspace/AC_HEAT/com_redox/src/main.cpp -Wall  -o /home/bbnewey/visual-workspace/AC_HEAT/com_redox/bin/main -lm -lredox_static -lev -lhiredis -pthread -ljsoncpp -I/home/bbnewey/visual-workspace/AC_HEAT/redox/include -L/home/bbnewey/visual-workspace/AC_HEAT/redox/build 