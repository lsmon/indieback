# Test CMakeLists.txt for verifying the configuration of the main CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(indieback_tests)

# Include the main CMakeLists.txt to test its configuration
include(${CMAKE_SOURCE_DIR}/CMakeLists.txt)

# Test if the library 'indieback' is created
if (TARGET indieback)
    message(STATUS "Test Passed: Target 'indieback' exists.")
else()
    message(FATAL_ERROR "Test Failed: Target 'indieback' does not exist.")
endif()

# Test if the executable 'indieback_test' is created
if (TARGET indieback_test)
    message(STATUS "Test Passed: Target 'indieback_test' exists.")
else()
    message(FATAL_ERROR "Test Failed: Target 'indieback_test' does not exist.")
endif()

# Test if the required include directories are set
get_target_property(INCLUDE_DIRS indieback INCLUDE_DIRECTORIES)
if (INCLUDE_DIRS)
    message(STATUS "Test Passed: Include directories are set for 'indieback'.")
else()
    message(FATAL_ERROR "Test Failed: Include directories are not set for 'indieback'.")
endif()

# Test if the required libraries are linked
get_target_property(LINK_LIBRARIES indieback LINK_LIBRARIES)
if (LINK_LIBRARIES)
    message(STATUS "Test Passed: Libraries are linked for 'indieback'.")
else()
    message(FATAL_ERROR "Test Failed: Libraries are not linked for 'indieback'.")
endif()