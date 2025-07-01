
include(CTest)
enable_testing()

if(OPENSSL_FOUND)

    if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        
        add_executable(indieback_rsa_test ${CMAKE_SOURCE_DIR}/tests/TestRSA.cpp ${INDIE_CRYPTO_INC} ${INDIE_CRYPTO_SRC})
        target_include_directories(indieback_rsa_test PUBLIC ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR})
        target_link_libraries(indieback_rsa_test PRIVATE ${OPENSSL_LIBRARIES}  OpenSSL::SSL OpenSSL::Crypto)
        add_test(NAME TEST_RSA COMMAND indieback_rsa_test)
        
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        add_executable(indieback_test ${CMAKE_SOURCE_DIR}/tests/TestIndieback.cpp ${INDIE_INC} ${INDIE_SRC} ${THIRD_PARTY_INC})
        target_include_directories(indieback_test PUBLIC ${CMAKE_SOURCE_DIR}/include)
        target_link_libraries(indieback_test indieback ${THIRD_PARTY_LIB})

        add_test(NAME IndieBackTest COMMAND indieback_test)
        # Add tests for each argument
        add_test(NAME TEST_CASSANDRA COMMAND indieback_test cassandra)
        add_test(NAME TEST_MODELS COMMAND indieback_test models)
        add_test(NAME TEST_CONTROLLERS COMMAND indieback_test controller)

        if(OPENSSL_FOUND)
            add_executable(indieback_rsa_test ${CMAKE_SOURCE_DIR}/tests/TestRSA.cpp ${INDIE_CRYPTO_INC} ${INDIE_CRYPTO_SRC} ${THIRD_PARTY_INC})
            target_include_directories(indieback_rsa_test PUBLIC ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR})
            target_link_libraries(indieback_rsa_test indieback ${THIRD_PARTY_LIB} ZLIB::ZLIB stdc++fs ${OPENSSL_LIBRARIES})

            add_executable(indieback_api_test ${CMAKE_SOURCE_DIR}/tests/TestApi.cpp ${INDIE_CRYPTO_INC} ${INDIE_CRYPTO_SRC} ${THIRD_PARTY_INC})
            target_include_directories(indieback_api_test PUBLIC ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR})
            target_link_libraries(indieback_api_test indieback ${THIRD_PARTY_LIB} ZLIB::ZLIB stdc++fs ${OPENSSL_LIBRARIES})
        endif()

        
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

    else()
        message(WARNING "Building on unknown OS: ${CMAKE_SYSTEM_NAME}")
    endif()

else()
    message(WARNING "OpenSSL not found, some features may be disabled")

endif()