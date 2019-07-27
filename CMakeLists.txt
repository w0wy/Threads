cmake_minimum_required(VERSION 3.7)
project(Threads)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)

set(BOOST_ROOT E:/boost_1_70_0/)
set(BOOST_INCLUDEDIR E:/boost_1_70_0/)
set(BOOST_LIBRARYDIR E:/boost_1_70_0/libs/)

find_package(BOOST 1.70.0 COMPONENTS system filesystem REQUIRED)
include_directories(${BOOST_INCLUDEDIR})

add_executable(Threads ${SOURCE_FILES})

target_link_libraries(Threads ${BOOST_LIBRARYDIR})