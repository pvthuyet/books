# http client
add_executable(http_sync_client http_sync_client.cpp)
target_include_directories(http_sync_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(http_sync_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

# websocket client
add_executable(websocket_sync_client websocket_sync_client.cpp)
target_include_directories(websocket_sync_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(websocket_sync_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

# http client sync ssl
add_executable(http_client_sync_ssl http_client_sync_ssl.cpp)
target_include_directories(http_client_sync_ssl PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(http_client_sync_ssl PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

# http sync server
add_executable(http_server_sync http_server_sync.cpp)
target_include_directories(http_server_sync PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(http_server_sync PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

# http sync server ssl
add_executable(http_server_sync_ssl http_server_sync_ssl.cpp)
target_include_directories(http_server_sync_ssl PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(http_server_sync_ssl PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})