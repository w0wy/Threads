#!/bin/bash
mkdir build
cd build
cmake ..
make
echo "---"
echo "-- Build successful!"
echo "-- SupervisorDaemon executable is in build directory."
echo "---"
