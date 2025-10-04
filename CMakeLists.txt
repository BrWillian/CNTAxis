cmake_minimum_required(VERSION 3.28)
project(CNTAxis)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
set(CMAKE_BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(OUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUT_DIRECTORY})

file(MAKE_DIRECTORY ${OUT_DIRECTORY})

find_package(OpenCV REQUIRED)
find_package(Iconv REQUIRED)
find_package(Threads REQUIRED)

set(LIB_DEPENDENCIES ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})

message(INFO ${LIB_DEPENDENCIES})

add_library(CNTAxis SHARED src/CNTAxis.cpp
        src/ONNXModel.cpp)

target_link_options(CNTAxis PRIVATE -static-libstdc++ -static-libgcc)
target_link_libraries(CNTAxis PRIVATE ${LIB_DEPENDENCIES})

set_target_properties(CNTAxis PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(CNTAxis PROPERTIES LINK_FLAGS "-shared -fPIC -Wl,-z,noexecstack")

add_definitions(-O2 -pthread)
