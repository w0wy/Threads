cmake_minimum_required(VERSION 3.7)
project(Threads)

add_subdirectory(Libs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt -std=c++14 -rdynamic -O3")

option(DEBUG "Turn on debug symbols and debug logs" OFF)
option(CODE_CHECK "Turn on code warnings check" OFF)

if (DEBUG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb3")
	set(CMAKE_BUILD_TYPE Debug)
endif()

if (CODE_CHECK)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie -Wall -Wextra -pedantic-errors") #-Werror")
endif()

set(SOURCE_FILES_DAEMON 
	SupervisorDaemon.cpp 
	SupervisorDaemon.h 
)

set(BOOST_INCLUDEDIR /usr/include/)
#set(BOOST_LIBRARYDIR /usr/lib/)

set(BOOST_COMPONENTS system thread date_time filesystem regex)
find_package(Boost 1.53 REQUIRED ${BOOST_COMPONENTS})

set(LIBS_INCLUDE_DIRS
	Libs/libMemory/include/
	Libs/libServices/include/
	Libs/libUtils/include/)

include_directories(${LIBS_INCLUDE_DIRS})

add_executable(SupervisorDaemon
	${SOURCE_FILES_DAEMON})

target_link_libraries(SupervisorDaemon
	utils
	services
	memory)