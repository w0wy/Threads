cmake_minimum_required(VERSION 3.7)
project(Threads)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp SyncUtils.h Logger.cpp Logger.h)
set(SOURCE_FILES_DAEMON SupervisorDaemon.cpp SupervisorDaemon.h Logger.cpp Logger.h)

set(BOOST_ROOT E:/boost_1_70_0/)
set(BOOST_INCLUDEDIR E:/boost_1_70_0/include/)
set(BOOST_LIBRARYDIR E:/boost_1_70_0/stageDir/lib/)
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
#set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

find_package(BOOST 1.70.0 COMPONENTS system filesystem REQUIRED)
include_directories(${BOOST_INCLUDEDIR})

add_executable(SupervisorDaemon ${SOURCE_FILES_DAEMON})
add_executable(Threads ${SOURCE_FILES})

target_link_libraries(Threads ${BOOST_LIBRARYDIR})
target_link_libraries(SupervisorDaemon ${BOOST_LIBRARYDIR})