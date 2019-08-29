#ifndef THREADS_SERVICERUNNER_H
#define THREADS_SERVICERUNNER_H

#include <stdio.h>
#include <stdlib.h>

namespace svc
{

#define RUN_SVCS_SCRIPT "\
	#/bin/bash \n\
	echo \"This is a test shell script inside CPP code!!\" \n\
	#./SupervisorDaemon \n\
	"

	void startServices()
	{
		system(RUN_SVCS_SCRIPT);
	}
}

#endif  // THREADS_SERVICERUNNER_H