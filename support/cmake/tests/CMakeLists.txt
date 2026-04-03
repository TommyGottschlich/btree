# Unit Tests CMakeLists.txt

# Collect all test files from the utests directory
file(GLOB_RECURSE TEST_SOURCES ${REPO_ROOT}/utests/*.cpp)
file(GLOB_RECURSE TEST_HEADERS ${REPO_ROOT}/utests/*.hpp)
# Include additional source but exclude main
file(GLOB_RECURSE SOURCES ${REPO_ROOT}/src/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Setup Boost unit testing
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
cmake_policy(SET CMP0167 NEW)
find_package(Boost 1.45.0 REQUIRED COMPONENTS unit_test_framework)

if(Boost_FOUND)
    message(STATUS "Boost found at: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found, but required for unit tests")
endif()

# Compile options for testings
add_compile_options(-DTESTING)
add_compile_options(-Wno-backslash-newline-escape)

# Iterate over all test source files
foreach(test_case ${TEST_SOURCES})
    # Get the base name of the test file (without extension) and prefix with utst_
    get_filename_component(test_name ${test_case} NAME_WE)

    # drop _test suffix if present
    string(REPLACE "_test" "" test_name "${test_name}")
    set(test_name "utst_${test_name}")

    # Create a unique executable for each test file
    add_executable(${test_name} ${test_case} ${SOURCES})

    # Link the test executable with libraries
    target_link_libraries(${test_name}
        PRIVATE ${Boost_LIBRARIES}
    )

    # Include directories for test files
    target_include_directories(${test_name}
        PRIVATE ${REPO_ROOT}/src
        PRIVATE ${REPO_ROOT}/inc
        PRIVATE ${REPO_ROOT}/utests
        PRIVATE ${Boost_INCLUDE_DIRS}
        PRIVATE ${RAPIDJSON_INCLUDE_DIRS}
    )

    # Add each test executable to CTest
    message("Adding test ${test_name}")
    add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

endforeach()