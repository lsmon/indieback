set(CASSANDRA_INC ${CMAKE_SOURCE_DIR}/include/cassandra.h)

set(CASSANDRA_LIB 
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.so 
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.so.2 
        ${CMAKE_SOURCE_DIR}/lib/libcassandra.so.${CASSANDRA_V})