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

## Bullet
add_subdirectory(lib/bullet-2.80)
link_libraries(BulletDynamics BulletCollision LinearMath)
include_directories(lib/bullet-2.80/src/)
## Boost
find_package(Boost COMPONENTS timer REQUIRED)
link_libraries(Boost::boost Boost::timer)

include_directories(
    metasolver
    metasolver/strategies/
    problems/clp/containers
    problems/clp/objects2
    problems/clp/evaluators
    problems/clp
)

add_library(
    bsg
    metasolver/SearchStrategy.cpp
    metasolver/GlobalVariables.cpp
    metasolver/strategies/BSG.cpp
    metasolver/strategies/Greedy.cpp
    metasolver/State.cpp
)

add_library(
    clp
    problems/clp/containers/AABBContainer.cpp
    problems/clp/containers/AABBTree.cpp
    problems/clp/evaluators/VCS_Function.cpp
    problems/clp/evaluators/VLossFunction.cpp
    problems/clp/objects2/AABB.cpp
    problems/clp/objects2/Block.cpp
    problems/clp/objects2/BoxShape.cpp
    problems/clp/objects2/Space.cpp
    problems/clp/objects2/SpaceSet.cpp
    problems/clp/objects2/Block_fsb.cpp
)

add_library(
    clp_state
    problems/clp/clpState.cpp
)

# Executables

add_executable(BSG_CLP problems/clp/main_clp.cpp)
target_link_libraries(BSG_CLP clp_state clp bsg)

# Tests

enable_testing()

add_executable(test_BSG_CLP problems/clp/test_clp.cpp)
target_link_libraries(test_BSG_CLP clp_state clp bsg)

add_test(
    NAME o-search
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR4.txt -i 1 --min_fr=0.98 -t 1 -s o-search --fp=R -f BRwp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME bsg
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR4.txt -i 1 --min_fr=0.98 -t 1 -s bsg --fp=R -f BRwp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME bsg_p
     COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR4.txt -i 1 --min_fr=0.98 -t 1 -s bsg_p --fp=R -f BRwp
     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME bsg_vp
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR4.txt -i 1 --min_fr=0.98 -t 2 -s bsg_vp --fp=R -f BRwp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
