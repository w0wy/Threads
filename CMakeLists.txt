cmake_minimum_required(VERSION 3.7)
project(Threads)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_FILES_DAEMON 
	SupervisorDaemon.cpp 
	SupervisorDaemon.h 
	Utils/Logger.cpp 
	Utils/Logger.h 
	Services/AService.h 
	Services/Service1.h 
	Services/Service1.cpp 
	Utils/MemoryHelpers.h)

set(BOOST_INCLUDEDIR /usr/include/)
set(BOOST_LIBRARYDIR /usr/lib/)

set(BOOST_COMPONENTS system thread date_time filesystem regex)
find_package(Boost 1.53 REQUIRED ${BOOST_COMPONENTS})

add_executable(SupervisorDaemon ${SOURCE_FILES_DAEMON})

target_link_libraries(SupervisorDaemon ${BOOST_LIBRARYDIR})