cmake_minimum_required(VERSION 3.10)
project(indieback)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_TWEAK 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")


include(external/CMakeLists.txt)
include(cmake/cache.cmake)
include(cmake/cassandra-driver.cmake)
include(cmake/netpp.cmake)
include(cmake/logpp.cmake)
include(cmake/json.cmake)
include(cmake/scheduler.cmake)

set(THIRD_PARTY_INC
        ${CASSANDRA_INC}
        ${NETPP_INC}
        ${LOGPP_INC}
        ${JSON_INC}
        ${CACHE_INC}
        ${SCHEDULER_INC})

set(THIRD_PARTY_LIB
        ${CASSANDRA_LIB}
        ${NETPP_LIB}
        ${LOGPP_LIB}
        ${JSON_LIB}
        ${CACHE_LIB}
        ${SCHEDULER_LIB})

set(BACKEND_INC ${CMAKE_SOURCE_DIR}/include/backend/CassandraConnection.hpp)
set(BACKEND_SRC ${CMAKE_SOURCE_DIR}/src/backend/CassandraConnection.cpp)

add_library(indieback ${BACKEND_SRC} ${BACKEND_INC} ${THIRD_PARTY_INC})
target_include_directories(indieback PUBLIC ${CMAKE_SOURCE_DIR}/include)
# target_link_libraries(indieback ${THIRD_PARTY_LIB})

include(CTest)
enable_testing()
add_executable(indieback_cassandra_connection_test ${CMAKE_SOURCE_DIR}/test/TestCassConnection.cpp ${BACKEND_SRC} ${BACKEND_INC} ${CASSANDRA_INC})
target_include_directories(indieback_cassandra_connection_test PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(indieback_cassandra_connection_test ${CASSANDRA_LIB})

add_test(TEST_CASS_CONNECTION indieback_cassandra_connection_test)