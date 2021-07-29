# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# fileio1
add_executable(dechat "dechat.cpp")
target_include_directories(dechat PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(dechat PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})