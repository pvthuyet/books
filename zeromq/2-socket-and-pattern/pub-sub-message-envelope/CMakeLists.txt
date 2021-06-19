# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(psenvpub psenvpub.cpp)
target_include_directories(psenvpub PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(psenvpub PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# client
add_executable(psenvsub psenvsub.cpp)
target_include_directories(psenvsub PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(psenvsub PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})