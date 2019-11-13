#include "gtest/gtest.h"
#include "SupervisorDaemon.h"
#include "SupervisorDaemon.cpp"

#define RUN_DAEMON "\
	#!/bin/bash \n\
	echo \"Starting services!\" \n\
	cd /var/fpwork/fduralia/masterpiece/Threads/exec/ \n\
	./SupervisorDaemon \n\
	"

struct SomeExampleTestFixture : public testing::Test
{
	void SetUp(){};
	void TearDown(){};
};

TEST_F(SomeExampleTestFixture, SomeExampleTestCase)
{
	std::cout << "damn son \n\n";
	pid_t pid, sid;

	pid = fork();

    if (pid > 0) { 
    	EXPECT_EQ(1,1);
    	std::cout << "current_process \n\n";
    }

    if (pid < 0) { exit(EXIT_FAILURE); }

    if (pid == 0)
    {
    	system(RUN_DAEMON);
    }
}