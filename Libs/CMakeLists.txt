####################################
#           LIBS SETUP             #
####################################
cmake_minimum_required(VERSION 3.7)
project(Libs VERSION 1.0.0 DESCRIPTION "threads library")

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt -fPIC")

set(BOOST_INCLUDEDIR /usr/include/)
set(BOOST_LIBRARYDIR /usr/lib/)

set(BOOST_COMPONENTS system thread date_time filesystem regex)
find_package(Boost 1.53 REQUIRED ${BOOST_COMPONENTS})

####################################
#            UTILS LIB             #
####################################
set(SOURCE_FILES_UTILS_LIBRARY
	libUtils/src/Logger.cpp
	)

add_library(utils SHARED ${SOURCE_FILES_UTILS_LIBRARY})

set_target_properties(utils PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(utils PROPERTIES SOVERSION 1)
set_target_properties(utils PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(utils PRIVATE libUtils/include/)

####################################
#            MEMORY LIB            #
####################################
set(SOURCE_FILES_MEMORY_LIBRARY
	libMemory/src/SharedMemoryManager.cpp
	)

add_library(memory SHARED ${SOURCE_FILES_MEMORY_LIBRARY})

set_target_properties(memory PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(memory PROPERTIES SOVERSION 1)
set_target_properties(memory PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(memory PRIVATE
	libUtils/include/
	libMemory/include/
	)
target_link_libraries(memory utils)

####################################
#          SERVICES LIB            #
####################################
set(SOURCE_FILES_SERVICES_LIBRARY
	libServices/src/ServiceRunner.cpp
	)

add_library(services SHARED ${SOURCE_FILES_SERVICES_LIBRARY})

set_target_properties(services PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(services PROPERTIES SOVERSION 1)
set_target_properties(services PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(services PRIVATE
	libUtils/include/
	libServices/include/
	)
target_link_libraries(services utils)

####################################
#            INSTALL               #
####################################
install(TARGETS utils services memory
	LIBRARY DESTINATION .
	PUBLIC_HEADER DESTINATION .)