#!/bin/bash

g++ -g /home/bbnewey/visual-workspace/OFFICE_LIGHTS/office_lights/src/tests.cpp -Wall  -o /home/bbnewey/visual-workspace/OFFICE_LIGHTS/office_lights/bin/tests -pthread -ljsoncpp -lmysqlclient -lboost_filesystem -I /usr/include/mysql 
