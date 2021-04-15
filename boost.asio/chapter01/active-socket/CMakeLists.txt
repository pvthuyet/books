# active-socket tcp
add_executable(active-socket-tcp "tcp.cpp")
target_include_directories(active-socket-tcp PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(active-socket-tcp PRIVATE ${Boost_LIBRARIES})

# active-socket udp
add_executable(active-socket-udp "udp.cpp")
target_include_directories(active-socket-udp PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(active-socket-udp PRIVATE ${Boost_LIBRARIES})
