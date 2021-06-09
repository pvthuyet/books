# blocking udp server
add_executable(daytime_client daytime_client.cpp)
target_include_directories(daytime_client PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(daytime_client PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})