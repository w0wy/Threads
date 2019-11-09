set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt -std=c++14 -rdynamic -O3")

set(TEST_SOURCE_FILES
	TestMain.cpp
	Utils_Tests.cpp
)

set(INCLUDE_FILES_UTILS
	../include/
)

include_directories(
	${INCLUDE_FILES_UTILS}
)

add_executable(utils-test
	${TEST_SOURCE_FILES}
)

add_test(NAME utils-test COMMAND utils-test)
target_link_libraries(utils-test PUBLIC
	utils-static
	gtest
	pthread
)