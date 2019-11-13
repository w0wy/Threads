set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt -std=c++14 -rdynamic -O3")

set(TEST_SOURCE_FILES
	TestMain.cpp
	ExampleTest.cpp
)

set(INCLUDE_FILES_DAEMON
	../include/
	../src/
)

#to be removed
set(LIBS_INCLUDE_DIRS
	../Libs/libMemory/include/
	../Libs/libServices/include/
	../Libs/libUtils/include/
)

include_directories(
	${INCLUDE_FILES_DAEMON}
	${LIBS_INCLUDE_DIRS} #to be removed
)

add_executable(SupervisorDaemon-test
	${TEST_SOURCE_FILES}
)

add_test(NAME SupervisorDaemon-test COMMAND SupervisorDaemon-test)
target_link_libraries(SupervisorDaemon-test PUBLIC
	SupervisorDaemon-static
	gtest
)