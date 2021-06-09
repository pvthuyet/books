add_executable(tracking custom_tracking.hpp async_tcp_echo_server.cpp)
target_include_directories(tracking PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(tracking PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})