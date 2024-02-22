cmake_minimum_required(VERSION 3.2)

#
# Project settings and generic definitions.
#
project(hflndp)

add_definitions(-Wall -Wextra -Wfloat-conversion -std=c++14 -pipe -march=x86-64 -mtune=generic)

# Debug and Release definitions.
# To set the build mode, specify the value of var `CMAKE_BUILD_TYPE` when
# running CMake.
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g")

# Fallback to Debug build type automatically
# if no compilation mode was specified.
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Debug")
endif()

message("-- Using compilation flags of mode \"" ${CMAKE_BUILD_TYPE} "\"")

# Include dependencies from CPLEX MIP solver.
include(cplex.cmake)


#
# Project compilation settings.
#

# Set the compiler to search for headers inside the source code dir.
include_directories(src)


add_executable(Healhcare-Facility-Locatio
   src/Instance.cpp
   src/main.cpp
   src/MipModel.cpp
)
target_link_libraries(Healhcare-Facility-Locatio ${CPLEX_LIBRARIES})