# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(hwserver server.cpp)
target_include_directories(hwserver PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(hwserver PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client
add_executable(hwclient "client.cpp")
target_include_directories(hwclient PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(hwclient PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# zmqpp test
add_executable(simple_client "simple_client.cpp")
target_include_directories(simple_client PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(simple_client PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})