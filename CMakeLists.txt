cmake_minimum_required(VERSION 3.10)
project(indieback)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
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

set(INDIE_CASS_CONN_INC 
        ${CMAKE_SOURCE_DIR}/include/backend/CassandraConnection.hpp)
set(INDIE_CASS_CONN_SRC 
        ${CMAKE_SOURCE_DIR}/src/backend/CassandraConnection.cpp)

include(CTest)
enable_testing()

add_executable(test_${PROJECT_NAME}_cass_conn ${CMAKE_SOURCE_DIR}/test/TestCassConnection.cpp ${INDIE_CASS_CONN_INC} ${INDIE_CASS_CONN_SRC} ${CASSANDRA_INC})
target_include_directories(test_${PROJECT_NAME}_cass_conn PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_${PROJECT_NAME}_cass_conn ${CASSANDRA_LIB})

add_test(TEST_CASS_CONNECTION 
        test_${PROJECT_NAME}_cass_conn)

set(INDIE_MODELS_INC
        ${CMAKE_SOURCE_DIR}/include/backend/IndieBackModels.hpp)
set(INDIE_MODELS_SRC
        ${CMAKE_SOURCE_DIR}/src/backend/IndieBackModels.cpp)


add_executable(test_${PROJECT_NAME}_models ${CMAKE_SOURCE_DIR}/test/TestModels.cpp ${INDIE_MODELS_INC} ${INDIE_MODELS_SRC} ${JSON_INC} ${NETPP_INC} ${LOGPP_INC} ${CACHE_INC} ${SCHEDULER_INC})
target_link_directories(test_${PROJECT_NAME}_models PUBLIC ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(test_${PROJECT_NAME}_models PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_${PROJECT_NAME}_models ${JSON_LIB} ${NETPP_LIB})
add_test(TEST_MODELS 
        test_${PROJECT_NAME}_models)


        


set(BACKEND_INC
        ${INDIE_CASS_CONN_INC})
set(BACKEND_SRC
        ${INDIE_CASS_CONN_SRC})

add_library(indieback ${BACKEND_SRC} ${BACKEND_INC} ${THIRD_PARTY_INC})
target_include_directories(indieback PUBLIC ${CMAKE_SOURCE_DIR}/include)
# target_link_libraries(indieback ${THIRD_PARTY_LIB})
