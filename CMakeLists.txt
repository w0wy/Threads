cmake_minimum_required(VERSION 3.7)
project(Threads)

add_subdirectory(Libs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt -std=c++14 -rdynamic -O3")

set(BOOST_INCLUDEDIR /usr/include/)
#set(BOOST_LIBRARYDIR /usr/lib/)

set(BOOST_COMPONENTS system thread date_time filesystem regex)
find_package(Boost 1.53 REQUIRED ${BOOST_COMPONENTS})

option(DEBUG "Turn on debug symbols and debug logs" OFF)
option(CODE_CHECK "Turn on code warnings check" OFF)
option(TEST_UTILS "Turn on build for tests of libUtils" OFF)
option(TEST_DAEMON "Turn on build for tests of Daemon" OFF)
option(TEST_MEMORY "Turn on build for tests of libMemory" OFF)

if (DEBUG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb3 -O0")
	set(CMAKE_BUILD_TYPE Debug)
endif()

if (CODE_CHECK)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie -Wall -Wextra -pedantic-errors") #-Werror")
endif()

if (TEST_DAEMON)
	add_subdirectory(Externals/googletest)
	add_subdirectory(test)
endif()

set(SOURCE_FILES_DAEMON 
	src/SupervisorDaemon.cpp 
)

set(INCLUDE_FILES_DAEMON
	include/
)

set(LIBS_INCLUDE_DIRS
	Libs/libMemory/include/
	Libs/libServices/include/
	Libs/libUtils/include/
	Libs/libXComm/include/
	Libs/libXComm/include/interfaces/
)

include_directories(
	${INCLUDE_FILES_DAEMON}
	${LIBS_INCLUDE_DIRS}
)

add_executable(SupervisorDaemon
	${SOURCE_FILES_DAEMON})

target_link_libraries(SupervisorDaemon
	utils
	services
	memory
	xcomm)

if (TEST_DAEMON)
	add_library(SupervisorDaemon-static STATIC
		${SOURCE_FILES_DAEMON})

	target_link_libraries(SupervisorDaemon-static
		utils
		services
		memory
		xcomm)
endif()
