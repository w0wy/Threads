#ifndef THREADS_SERVICERUNNER_H
#define THREADS_SERVICERUNNER_H

#include <stdio.h>
#include <stdlib.h>

#include "Logger.h"

namespace svc
{

#define RUN_SVCS_SCRIPT "\
	#!/bin/bash \n\
	echo \"Starting services!\" \n\
	#./SupervisorDaemon \n\
	"

void startServices();

}

#endif  // THREADS_SERVICERUNNER_H