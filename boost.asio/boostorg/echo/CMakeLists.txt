# blocking udp server
add_executable(blocking_udp_server blocking_udp_echo_server.cpp)
target_include_directories(blocking_udp_server PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_udp_server PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
# blocking udp client
add_executable(blocking_udp_echo_client blocking_udp_echo_client.cpp)
target_include_directories(blocking_udp_echo_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_udp_echo_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

# blocking tcp server
add_executable(blocking_tcp_echo_server blocking_tcp_echo_server.cpp)
target_include_directories(blocking_tcp_echo_server PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_tcp_echo_server PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
# blocking tcp client
add_executable(blocking_tcp_echo_client blocking_tcp_echo_client.cpp)
target_include_directories(blocking_tcp_echo_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_tcp_echo_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

# async udp server
add_executable(async_udp_echo_server async_udp_echo_server.cpp)
target_include_directories(async_udp_echo_server PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(async_udp_echo_server PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
# async tcp server
add_executable(async_tcp_echo_server async_tcp_echo_server.cpp)
target_include_directories(async_tcp_echo_server PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(async_tcp_echo_server PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})