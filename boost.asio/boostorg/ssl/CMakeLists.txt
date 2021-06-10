add_executable(clientssl client.cpp)
target_include_directories(clientssl PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(clientssl PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

add_executable(serverssl server.cpp)
target_include_directories(serverssl PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(serverssl PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})