# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(rrclient rrclient.cpp)
target_include_directories(rrclient PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(rrclient PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# server
add_executable(rrworker rrworker.cpp)
target_include_directories(rrworker PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(rrworker PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# broker
add_executable(rrbroker rrbroker.cpp)
target_include_directories(rrbroker PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(rrbroker PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# message queue
add_executable(msgqueue msgqueue.cpp)
target_include_directories(msgqueue PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(msgqueue PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})