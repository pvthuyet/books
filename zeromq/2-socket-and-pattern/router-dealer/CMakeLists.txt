# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(rrclient rrclient.cpp)
target_include_directories(rrclient PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(rrclient PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server
add_executable(rrworker rrworker.cpp)
target_include_directories(rrworker PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(rrworker PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# broker
add_executable(rrbroker rrbroker.cpp)
target_include_directories(rrbroker PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(rrbroker PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# message queue
add_executable(msgqueue msgqueue.cpp)
target_include_directories(msgqueue PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(msgqueue PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})