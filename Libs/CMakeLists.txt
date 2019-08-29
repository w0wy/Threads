cmake_minimum_required(VERSION 3.7)
project(Libs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt")

set(SOURCE_FILES_UTILS_LIBRARY
	UtilsLib/Logger.cpp 
	UtilsLib/Logger.h
	UtilsLib/MemoryHelpers.h)

set(SOURCE_FILES_SERVICES_LIBRARY
	ServicesLib/ServiceRunner.h
	ServicesLib/AService.h)

set(BOOST_INCLUDEDIR /usr/include/)
set(BOOST_LIBRARYDIR /usr/lib/)

set(BOOST_COMPONENTS system thread date_time filesystem regex)
find_package(Boost 1.53 REQUIRED ${BOOST_COMPONENTS})

add_library(utils.so ${SOURCE_FILES_UTILS_LIBRARY})
add_library(services.so ${SOURCE_FILES_SERVICES_LIBRARY})

target_link_libraries(services.so utils.so)
