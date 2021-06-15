# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(hwserver server.cpp)
target_include_directories(hwserver PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(hwserver PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# client
add_executable(hwclient "client.cpp")
target_include_directories(hwclient PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(hwclient PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})