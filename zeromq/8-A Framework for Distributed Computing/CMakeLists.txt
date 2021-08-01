# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# dechat
add_executable(dechat "dechat.cpp")
target_include_directories(dechat PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(dechat PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# udpping1
add_executable(udpping1 "udpping1.cpp")
target_include_directories(udpping1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(udpping1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# udpping2
add_executable(udpping2 "udpping2.cpp" "udplib.cpp")
target_include_directories(udpping2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(udpping2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})