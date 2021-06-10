add_executable(spawn echo_server.cpp)
target_include_directories(spawn PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(spawn PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})