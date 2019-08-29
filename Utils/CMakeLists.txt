cmake_minimum_required(VERSION 3.7)
project(UtilsLibrary)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt")

set(SOURCE_FILES_UTILS_LIBRARY
	Logger.cpp 
	Logger.h
	MemoryHelpers.h)

set(BOOST_INCLUDEDIR /usr/include/)
set(BOOST_LIBRARYDIR /usr/lib/)

set(BOOST_COMPONENTS system thread date_time filesystem regex)
find_package(Boost 1.53 REQUIRED ${BOOST_COMPONENTS})

add_library(UtilsLibrary ${SOURCE_FILES_UTILS_LIBRARY})
#add_library(UtilsLibrary OBJECT ${SOURCE_FILES_UTILS_LIBRARY})