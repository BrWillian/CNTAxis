cmake_minimum_required(VERSION 3.0)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

add_library(untitled SHARED src/CNTAxis.cpp meta/types.h
        include/CNTAxis.h)
