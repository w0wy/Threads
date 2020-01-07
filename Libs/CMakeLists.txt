####################################
#           LIBS SETUP             #
####################################
cmake_minimum_required(VERSION 3.7)
project(Libs VERSION 1.0.0 DESCRIPTION "threads library")

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -lrt")

set(BOOST_INCLUDEDIR /usr/include/)
set(BOOST_LIBRARYDIR /usr/lib/)

set(BOOST_COMPONENTS system thread date_time filesystem regex)
find_package(Boost 1.53 REQUIRED ${BOOST_COMPONENTS})

if (DEBUG)
	add_definitions(-D__DEBUG__)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb3 -O0")
	set(CMAKE_BUILD_TYPE Debug)
endif()

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

if (TEST_UTILS)
	add_library(utils-static STATIC
		${SOURCE_FILES_UTILS_LIBRARY}
	)
	include_directories(../Externals/googletest)
	add_subdirectory(libUtils/test/)
	target_include_directories(utils-static PRIVATE libUtils/include/)
endif()

####################################
#            MEMORY LIB            #
####################################
set(SOURCE_FILES_MEMORY_LIBRARY
	libMemory/src/MemoryManager.cpp
)

add_library(memory SHARED ${SOURCE_FILES_MEMORY_LIBRARY})

set_target_properties(memory PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(memory PROPERTIES SOVERSION 1)
set_target_properties(memory PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(memory PRIVATE
	libUtils/include/
	libMemory/include/
	)
target_link_libraries(memory rt utils)

if (TEST_MEMORY)
	add_library(memory-static STATIC
		${SOURCE_FILES_MEMORY_LIBRARY}
	)
	include_directories(../External/googletest)
	add_subdirectory(libMemory/test/)
	target_link_libraries(memory-static utils)
	target_include_directories(memory-static PRIVATE
		libMemory/include/
		libUtils/include/)
endif()

####################################
#            XCOMM LIB            #
####################################
set(SOURCE_FILES_XCOMM_LIBRARY
	libXComm/src/Dummy.cpp
)

add_library(xcomm SHARED ${SOURCE_FILES_XCOMM_LIBRARY})

set_target_properties(xcomm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(xcomm PROPERTIES SOVERSION 1)
set_target_properties(xcomm PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(xcomm PRIVATE
	libXComm/include/
	libXComm/include/interfaces	
	libUtils/include/
	libMemory/include/
	)
target_link_libraries(xcomm utils memory)

if (TEST_XCOMM)
	add_library(xcomm-static STATIC
		${SOURCE_FILES_XCOMM_LIBRARY}
	)
	include_directories(../External/googletest)
	add_subdirectory(libXComm/test/)
	target_link_libraries(xcomm-static utils memory)
	target_include_directories(xcomm-static PRIVATE
		libXComm/include/
		libXComm/include/interfaces
		libUtils/include/
		libMemory/include/
		)
endif()

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

