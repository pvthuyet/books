# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(wuserver wuserver.cpp)
target_include_directories(wuserver PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(wuserver PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# client
add_executable(wuclient wuclient.cpp)
target_include_directories(wuclient PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(wuclient PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# proxy
add_executable(wuproxy wuproxy.cpp)
target_include_directories(wuproxy PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(wuproxy PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})