set(CASSANDRA_INC ${CMAKE_SOURCE_DIR}/include/cassandra.h)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(CASSANDRA_LIB 
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.dylib
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.2.dylib
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.${CASSANDRA_V}.dylib)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(CASSANDRA_LIB 
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.so 
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.so.2 
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.so.${CASSANDRA_V})
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

else()

endif()