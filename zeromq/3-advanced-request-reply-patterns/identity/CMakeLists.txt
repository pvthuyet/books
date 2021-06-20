# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(identity "identity.cpp")
target_include_directories(identity PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(identity PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})