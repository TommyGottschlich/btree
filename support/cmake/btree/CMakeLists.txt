# Source CMakeLists.txt

# Collect all source files from the src directory
file(GLOB_RECURSE SOURCES ${REPO_ROOT}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${REPO_ROOT}/inc/*.hpp)

# Add the executable for the main project
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories for the executable
target_include_directories(${PROJECT}
    PRIVATE
        ${REPO_ROOT}/src
        ${REPO_ROOT}/inc
)

