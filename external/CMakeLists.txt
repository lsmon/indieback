message(STATUS "Project root path: ${CMAKE_SOURCE_DIR}")
message(STATUS "installing netpp")

set(NETPP_V "0.1.0.3")

set(netpp ${CMAKE_SOURCE_DIR}/netpp)
if (NOT EXISTS ${netpp})
    message(STATUS "${netpp} is NOT installed")
    execute_process(COMMAND python3 ${CMAKE_SOURCE_DIR}/external/install-scripts/install_netpp.py ${CMAKE_SOURCE_DIR} ${NETPP_V})
else ()
    message(STATUS "${netpp} is installed")
endif ()

set(NETPP_INC ${CMAKE_SOURCE_DIR}/include)
set(NETPP_LIB ${CMAKE_SOURCE_DIR}/lib/lib_netpp-${NETPP_V}.a)

message(STATUS "logpp installing")
set(LOGPP_V "0.9.2.5")
set(logpp ${CMAKE_SOURCE_DIR}/logpp)
if (NOT EXISTS ${logpp})
    message(STATUS "${logpp} is NOT installed")
    execute_process(COMMAND python3 ${CMAKE_SOURCE_DIR}/netpp/external/install-scripts/install_logpp.py ${CMAKE_SOURCE_DIR} ${LOGPP_V})
else ()
    message(STATUS "${logpp} is installed")
endif ()

set(LOGPP_INC ${CMAKE_SOURCE_DIR}/include)
set(LOGPP_LIB ${CMAKE_SOURCE_DIR}/lib/lib_logpp-${LOGPP_V}.a)


# Adding json project
message(STATUS "Project root path: ${CMAKE_SOURCE_DIR}")

message(STATUS "json installing")
set(JSON_V "0.3.0.0")
set(json ${CMAKE_SOURCE_DIR}/json)
if (NOT EXISTS ${json})
    message(STATUS "${json} is NOT installed")
    execute_process(COMMAND python3 ${CMAKE_SOURCE_DIR}/netpp/external/install-scripts/install_json.py ${CMAKE_SOURCE_DIR} ${JSON_V})
else ()
    message(STATUS "logpp is installed")
endif ()

set(JSON_INC ${CMAKE_SOURCE_DIR}/include)
set(JSON_LIB ${CMAKE_SOURCE_DIR}/lib/lib_json-${JSON_V}.a)

message(STATUS "caching installing")
set(CACHE_V "0.1.0.0")
set(cache ${CMAKE_SOURCE_DIR}/cache)
if (NOT EXISTS ${cache})
    message(STATUS "${cache} is NOT installed")
    execute_process(COMMAND python3 ${CMAKE_SOURCE_DIR}/external/install-scripts/install_cache.py ${CMAKE_SOURCE_DIR} ${CACHE_V})
else ()
    message(STATUS "${cache} is installed")
endif ()

set(CACHE_INC ${CMAKE_SOURCE_DIR}/include)
set(CACHE_LIB ${CMAKE_SOURCE_DIR}/lib/lib_caching-${CACHE_V}.a)


message(STATUS "scheduler installing")
set(SCHEDULER_V "0.1.0.0")
message(STATUS "scheduler is NOT installed")
set(scheduler ${CMAKE_SOURCE_DIR}/cassandra-cpp-driver)
if (NOT EXISTS ${scheduler})
    execute_process(COMMAND python3 ${CMAKE_SOURCE_DIR}/external/install-scripts/install_scheduler.py ${CMAKE_SOURCE_DIR} ${SCHEDULER_V})
else ()
    message(STATUS "${scheduler} is installed")
endif ()

set(SCHEDULER_INC ${CMAKE_SOURCE_DIR}/include)
set(SCHEDULER_LIB ${CMAKE_SOURCE_DIR}/lib/lib_scheduler-${SCHEDULER_V}.a)


message(STATUS "cassandra cpp driver installing")
set(CASSANDRA_V "2.17.1")
message(STATUS "cassandra is NOT installed")
set(cassandra ${CMAKE_SOURCE_DIR}/cassandra-cpp-driver)

if (NOT EXISTS ${cassandra})
    execute_process(COMMAND python3 ${CMAKE_SOURCE_DIR}/external/install-scripts/install_cassandra.py ${CMAKE_SOURCE_DIR} ${CASSANDRA_V})
else ()
    message(STATUS "${cassandra} is installed")
endif ()

set(CASSANDRA_INC ${CMAKE_SOURCE_DIR}/include)
set(CASSANDRA_LIB 
    ${CMAKE_SOURCE_DIR}/lib/libcassandra.so.${CASSANDRA_V}
    ${CMAKE_SOURCE_DIR}/lib/libcassandra.so.2
    ${CMAKE_SOURCE_DIR}/lib/libcassandra.so)