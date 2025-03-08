cmake_minimum_required(VERSION 3.10)

# Project name
project(DirectToTheFuckingScreen)

# Set the source directory
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# Find all .cpp files in the src directory
file(GLOB CPP_SOURCES "${SRC_DIR}/*.cpp")

# Create the executable using only the .cpp files
add_executable(kube ${CPP_SOURCES})

# Set C++ standard
set_target_properties(kube PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
