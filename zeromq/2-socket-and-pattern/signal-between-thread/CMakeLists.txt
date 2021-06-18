# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(mtreply "mtreply.cpp")
target_include_directories(mtreply PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(mtreply PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})