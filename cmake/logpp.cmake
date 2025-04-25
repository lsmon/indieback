
set(LOG_INC_UTIL_FIO 
        include/util/fio/LogAppender.hpp
        include/util/fio/PropertiesReader.hpp)

set(LOG_INC_UTIL_LOGGING 
        include/util/logging/Level.hpp
        include/util/logging/Log.hpp)

set(LOG_INC_UTIL_PROPERTIES 
        include/util/properties/LogProperties.hpp)

set(LOG_INC_UTIL
        include/util/Date.hpp
        include/util/LogUtil.hpp)

set(LOG_INC 
        ${LOG_INC_UTIL_FIO}
        ${LOG_INC_UTIL_LOGGING}
        ${LOG_INC_UTIL_PROPERTIES}
        ${LOG_INC_UTIL}      
        include/logconfig.h)

set(LOGPP_LIBRARY ${CMAKE_SOURCE_DIR}/lib/lib_logpp-0.9.2.5.a)

message(STATUS "LOGPP_LIBRARY: ${LOGPP_LIBRARY}")
message(STATUS "LOG_INC: ${LOG_INC}")