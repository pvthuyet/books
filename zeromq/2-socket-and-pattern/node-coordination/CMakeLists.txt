# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(syncpub syncpub.cpp)
target_include_directories(syncpub PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(syncpub PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# client
add_executable(syncsub syncsub.cpp)
target_include_directories(syncsub PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(syncsub PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})