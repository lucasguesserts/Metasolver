CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

project(MetaSolver)

# Prevent building in the source directory

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()

# Build options

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Dependencies

## Args
include_directories(dependencies/args/)
## Bullet
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w") # disable compile warnings for bullet
add_subdirectory(dependencies/bullet-2.80)
link_libraries(BulletDynamics BulletCollision LinearMath)
include_directories(dependencies/bullet-2.80/src/)

## Boost
find_package(Boost COMPONENTS timer REQUIRED)
link_libraries(Boost::boost Boost::timer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

include_directories(
    src/
    src/metasolver
    src/metasolver/strategies/
    src/containers
    src/objects
    src/evaluators
)

add_library(
    clp
    src/containers/AABBContainer.cpp
    src/containers/AABBTree.cpp
    src/evaluators/VCS_Function.cpp
    src/evaluators/VLossFunction.cpp
    src/objects/AABB.cpp
    src/objects/Block.cpp
    src/objects/BoxShape.cpp
    src/objects/Space.cpp
    src/objects/SpaceSet.cpp
    src/objects/Block_fsb.cpp
    src/metasolver/SearchStrategy.cpp
    src/metasolver/GlobalVariables.cpp
    src/metasolver/strategies/BSG.cpp
    src/metasolver/strategies/Greedy.cpp
    src/metasolver/State.cpp
    src/metasolver/clpState.cpp
)

add_executable(BSG_CLP src/main.cpp)
target_link_libraries(BSG_CLP clp)
