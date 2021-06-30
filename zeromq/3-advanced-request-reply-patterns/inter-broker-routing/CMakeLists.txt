# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(peering1 "peering1.cpp")
target_include_directories(peering1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(peering1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client
add_executable(peering2_client "peering2_client.cpp")
target_include_directories(peering2_client PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(peering2_client PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server
add_executable(peering2_server "peering2_server.cpp")
target_include_directories(peering2_server PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(peering2_server PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})