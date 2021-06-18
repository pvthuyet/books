# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(mtserver mtserver.cpp)
target_include_directories(mtserver PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(mtserver PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})