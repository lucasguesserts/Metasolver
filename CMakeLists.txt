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
add_subdirectory(dependencies/bullet-2.80)
link_libraries(BulletDynamics BulletCollision LinearMath)
include_directories(dependencies/bullet-2.80/src/)
## Boost
find_package(Boost COMPONENTS timer REQUIRED)
link_libraries(Boost::boost Boost::timer)

include_directories(
    src/metasolver
    src/metasolver/strategies/
    src/clp/containers
    src/clp/objects2
    src/clp/evaluators
    src/clp
)

add_library(
    bsg
    src/metasolver/SearchStrategy.cpp
    src/metasolver/GlobalVariables.cpp
    src/metasolver/strategies/BSG.cpp
    src/metasolver/strategies/Greedy.cpp
    src/metasolver/State.cpp
)

add_library(
    clp
    src/clp/containers/AABBContainer.cpp
    src/clp/containers/AABBTree.cpp
    src/clp/evaluators/VCS_Function.cpp
    src/clp/evaluators/VLossFunction.cpp
    src/clp/objects2/AABB.cpp
    src/clp/objects2/Block.cpp
    src/clp/objects2/BoxShape.cpp
    src/clp/objects2/Space.cpp
    src/clp/objects2/SpaceSet.cpp
    src/clp/objects2/Block_fsb.cpp
)

add_library(
    clp_state
    src/clp/clpState.cpp
)

# Executables

add_executable(BSG_CLP src/main.cpp)
target_link_libraries(BSG_CLP clp_state clp bsg)
