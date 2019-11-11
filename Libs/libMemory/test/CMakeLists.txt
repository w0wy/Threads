set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt -std=c++14 -rdynamic -O3")

set(TEST_SOURCE_FILES
	TestMain.cpp
	Memory_Tests.cpp
)

set(INCLUDE_FILES_MEMORY_MANAGER
	../include/
)

set(INCLUDE_FILES_UTILS_LIB
	../../libUtils/include/
)

include_directories(
	${INCLUDE_FILES_MEMORY_MANAGER}
	${INCLUDE_FILES_UTILS_LIB}
)

add_executable(memory-test
	${TEST_SOURCE_FILES}
)

add_test(NAME memory-test COMMAND memory-test)
target_link_libraries(memory-test PUBLIC
	memory-static
	utils
	gtest
	pthread
)