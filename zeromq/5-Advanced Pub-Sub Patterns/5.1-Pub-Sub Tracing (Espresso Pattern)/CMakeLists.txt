# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(espresso "espresso.cpp" "publisher.hpp" "subscriber.hpp" "listener.hpp")
target_include_directories(espresso PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(espresso PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

