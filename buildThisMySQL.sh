#!/bin/bash

g++ -Wall  -o /home/bbnewey/visual-workspace/AC_HEAT/com_redox/bin/main /home/bbnewey/visual-workspace/AC_HEAT/com_redox/src/main.cpp -I/usr/include/mongo-cxx-driver-v2.4/src/ -I/usr/include/mongo-cxx-driver-v2.4/ -L/usr/lib -lm  -lev -pthread -lmongoclient  -lboost_thread -lboost_filesystem -lboost_program_options -lboost_system