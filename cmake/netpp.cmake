set(NETPP_INC
        include/http/ContentType.hpp
        include/http/Server.hpp
        include/http/Method.hpp
        include/http/Handler.hpp
        include/http/Structures.hpp
        include/http/Request.hpp
        include/http/Response.hpp
        include/http/Status.hpp
        include/http/QueryString.hpp
        include/http/WebSocketFrame.hpp
        include/util/URL.hpp
        include/http/Path.hpp
        include/Exception.hpp
        include/util/UUID.hpp
        include/util/String.hpp
        include/api/Client.hpp
        include/util/URL.hpp
        include/netppconfig.h)

        set(NETPP_LIBRARY ${CMAKE_SOURCE_DIR}/lib/lib_netpp-0.1.0.3.a) 