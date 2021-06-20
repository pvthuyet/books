# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(rtreq "rtreq.cpp")
target_include_directories(rtreq PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(rtreq PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})