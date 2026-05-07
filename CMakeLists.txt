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

execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND git describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE CNTAXIS_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")
add_definitions(-DCNTAXIS_VERSION="${CNTAXIS_VERSION}")

message(STATUS "Generating version.h")

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/meta/Version.h
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Version.h
)

message(STATUS "Generating weights.h")

execute_process(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/meta/
        COMMAND tar xf weights.tar.gz
)

execute_process(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/meta/
        COMMAND xxd -i cntaxis.onnx
        OUTPUT_VARIABLE CNTAXIS_WEIGHTS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/meta/Weights.h
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/Weights.h
)

execute_process(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/meta/
        COMMAND rm cntaxis.onnx
)


set(ONNX_RUNTIME_LIB_STATIC /usr/local/lib/libonnxruntime.a)

if(NOT EXISTS ${ONNX_RUNTIME_LIB_STATIC})
    message(FATAL_ERROR "ONNX Runtime static library not found: ${ONNX_RUNTIME_LIB_STATIC}")
endif()

find_package(OpenCV REQUIRED)
find_package(Iconv REQUIRED)
find_package(Threads REQUIRED)

set(LIB_DEPENDENCIES ${ONNX_RUNTIME_LIB_STATIC} ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})

message(INFO ${LIB_DEPENDENCIES})

add_library(CNTAxis SHARED src/CNTAxis.cpp
        src/ONNXModel.cpp
        src/ImageOp.cpp
        src/CNTCore.cpp
        src/C_Wrapper.cpp)

target_link_options(CNTAxis PRIVATE -static-libstdc++ -static-libgcc)
target_compile_options(CNTAxis PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
target_link_options(CNTAxis PRIVATE -Wl,--exclude-libs,ALL)

add_executable(debug tests/debug.cpp)
target_link_libraries(CNTAxis PRIVATE ${LIB_DEPENDENCIES})
target_link_libraries(debug PRIVATE CNTAxis ${LIB_DEPENDENCIES})

set_target_properties(CNTAxis PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(CNTAxis PROPERTIES LINK_FLAGS "-shared -fPIC -Wl,-z,noexecstack")

add_definitions(-O2 -pthread)