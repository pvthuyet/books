add_executable(toserver server.cpp)
target_include_directories(toserver PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(toserver PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

add_executable(blocking_udp_client blocking_udp_client.cpp)
target_include_directories(blocking_udp_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_udp_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

add_executable(blocking_tcp_client blocking_tcp_client.cpp)
target_include_directories(blocking_tcp_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_tcp_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

add_executable(blocking_token_tcp_client blocking_token_tcp_client.cpp)
target_include_directories(blocking_token_tcp_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(blocking_token_tcp_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

add_executable(async_tcp_client async_tcp_client.cpp)
target_include_directories(async_tcp_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(async_tcp_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})