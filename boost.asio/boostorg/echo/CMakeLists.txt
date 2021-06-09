add_executable(blocking_udp_server blocking_udp_echo_server.cpp)
target_include_directories(blocking_udp_server PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_udp_server PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

add_executable(blocking_udp_echo_client blocking_udp_echo_client.cpp)
target_include_directories(blocking_udp_echo_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_udp_echo_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})