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

# client 2
add_executable(flclient2 "flclient2.cpp")
target_include_directories(flclient2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(flclient2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server 2
add_executable(flserver2 "flserver2.cpp")
target_include_directories(flserver2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(flserver2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client 3
add_executable(flclient3 "flclient3.cpp" "flcapi.hpp")
target_include_directories(flclient3 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(flclient3 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server 3
add_executable(flserver3 "flserver3.cpp")
target_include_directories(flserver3 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(flserver3 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})