# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.30)

set(PROJECT btree)
# Set the project name
project (${PROJECT} C CXX)

# Set C99 and C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # disables GNU extensions, use pure C++20
# set(CMAKE_C_STANDARD 99)

# REPO_ROOT is passed in from the build script via -DREPO_ROOT=
# Fall back to deriving it from CMAKE_SOURCE_DIR when run directly (e.g. CLion)
if(NOT REPO_ROOT)
    cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SUPPORT_DIR)  # support/
    cmake_path(GET SUPPORT_DIR            PARENT_PATH REPO_ROOT)    # repo/
endif()

enable_testing()

# Include subdirectories for main project and tests
add_subdirectory(${REPO_ROOT}/support/cmake/btree ${CMAKE_BINARY_DIR}/btree)

# Include test subdirectory
add_subdirectory(${REPO_ROOT}/support/cmake/tests ${CMAKE_BINARY_DIR}/tests)
