# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(flclient1 "flclient1.cpp")
target_include_directories(flclient1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(flclient1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server
add_executable(flserver1 "flserver1.cpp")
target_include_directories(flserver1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(flserver1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})