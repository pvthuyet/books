# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(suisnail "suisnail.cpp")
target_include_directories(suisnail PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(suisnail PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})