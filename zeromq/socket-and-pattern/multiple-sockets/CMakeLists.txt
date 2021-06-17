# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(msreader msreader.cpp)
target_include_directories(msreader PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(msreader PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

add_executable(mspoller mspoller.cpp)
target_include_directories(mspoller PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(mspoller PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})