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

set(PROPERTY ${CMAKE_BINARY_DIR}/resources/logging.properties)
set(LOG_CONFIG ${CMAKE_SOURCE_DIR}/include/logconfig.h)
set(CONFIG ${CMAKE_SOURCE_DIR}/include/config.h)

set(CMK_SRC_DIR ${CMAKE_SOURCE_DIR})

message(STATUS "Build TYPE: ${CMAKE_BUILD_TYPE}")
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(DEBUG_MODE "DEBUG")
endif()


configure_file(logconfig.h.in ${LOG_CONFIG})
configure_file(logging.properties.in ${PROPERTY})
configure_file(config.h.in ${CONFIG})

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


set(INDIE_INC 
        ${CMAKE_SOURCE_DIR}/include/backend/CassandraConnection.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/IndieBackModels.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/User.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/Venue.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/Band.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/BandMember.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/EventByVenue.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/TicketByEvent.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/TicketByUser.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/PostsByDate.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/DailyTicketSales.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/models/Credentials.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/UsersController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/VenuesController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/BandsController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/BandMembersController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/EventController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/PostsByDateController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/TicketsByEventController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/TicketsByUserController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/DailyTicketSalesController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/controllers/CredentialsController.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/api/Endpoints.hpp
        ${CMAKE_SOURCE_DIR}/include/backend/api/RESTfulAPI.hpp)

set(INDIE_CRYPTO_INC 
        ${CMAKE_SOURCE_DIR}/include/crypto/AuthCrypto.hpp
        ${CMAKE_SOURCE_DIR}/include/crypto/Hash.hpp
        ${CMAKE_SOURCE_DIR}/include/crypto/RsaClient.hpp
        ${CMAKE_SOURCE_DIR}/include/crypto/RsaServer.hpp
        ${CMAKE_SOURCE_DIR}/include/crypto/StringEncoder.hpp)

set(INDIE_SRC 
        ${CMAKE_SOURCE_DIR}/src/backend/CassandraConnection.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/IndieBackModels.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/User.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/Venue.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/Band.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/BandMember.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/EventByVenue.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/TicketByEvent.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/TicketByUser.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/PostsByDate.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/Credentials.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/models/DailyTicketSales.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/UsersController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/VenuesController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/BandsController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/BandMembersController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/EventController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/PostsByDateController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/TicketsByEventController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/TicketsByUserController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/DailyTicketSalesController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/controllers/CredentialsController.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/api/Endpoints.cpp
        ${CMAKE_SOURCE_DIR}/src/backend/api/RESTfulAPI.cpp)

set(INDIE_CRYPTO_SRC 
        ${CMAKE_SOURCE_DIR}/src/crypto/AuthCrypto.cpp
        ${CMAKE_SOURCE_DIR}/src/crypto/RsaClient.cpp
        ${CMAKE_SOURCE_DIR}/src/crypto/RsaServer.cpp
        ${CMAKE_SOURCE_DIR}/src/crypto/Hash.cpp
        ${CMAKE_SOURCE_DIR}/src/crypto/StringEncoder.cpp)
    
find_package(ZLIB REQUIRED)

find_package(OpenSSL REQUIRED)

if(OPENSSL_FOUND)
    message(STATUS "OPENSSL_INCLUDE_DIR:    ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OPENSSL_LIBRARIES:      ${OPENSSL_LIBRARIES}")
    
    include_directories(${OPENSSL_INCLUDE_DIR})
    
    add_library(indie_rsa ${INDIE_CRYPTO_INC} ${INDIE_CRYPTO_SRC})
    target_include_directories(indie_rsa PUBLIC ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR})

    add_library(indieback ${INDIE_INC} ${INDIE_SRC} ${THIRD_PARTY_INC})
    target_include_directories(indieback PUBLIC ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR})

    add_executable(rest_api ${CMAKE_SOURCE_DIR}/src/APIServer.cpp ${INDIE_INC} ${INDIE_SRC} ${INDIE_CRYPTO_INC} ${INDIE_CRYPTO_SRC} ${THIRD_PARTY_INC})
    target_include_directories(rest_api PUBLIC ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR})

    if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        message(STATUS "Building on macOS")

        target_link_libraries(indie_rsa PRIVATE ${OPENSSL_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
        
        target_compile_definitions(indie_rsa PUBLIC OPENSSL_SYS_MACOSX)

        set_target_properties(indie_rsa PROPERTIES
                CXX_STANDARD 17
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
                OUTPUT_NAME "indie_rsa"
                PREFIX "lib"
                SUFFIX ".dylib")

        target_link_libraries(rest_api indieback indie_rsa ${THIRD_PARTY_LIB} ${OPENSSL_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)

    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        message(STATUS "Building on Linux")

        target_link_libraries(indie_rsa ${OPENSSL_LIBRARIES} ZLIB::ZLIB stdc++fs ${OPENSSL_LIBRARIES})

        target_link_libraries(indieback indie_rsa ${THIRD_PARTY_LIB} ZLIB::ZLIB stdc++fs ${OPENSSL_LIBRARIES})

        target_link_libraries(rest_api indieback indie_rsa ${THIRD_PARTY_LIB} ZLIB::ZLIB stdc++fs ${OPENSSL_LIBRARIES})

    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        message(WARNING "Building on Windows")
    else()
        message(WARNING "Building on unknown OS: ${CMAKE_SYSTEM_NAME}")
    endif()

else()
    message(WARNING "OpenSSL not found, some features may be disabled")
endif()

include(tests/CMakeLists.txt)

