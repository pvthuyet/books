# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(pathopub "pathopub.cpp")
target_include_directories(pathopub PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(pathopub PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client
add_executable(pathosub "pathosub.cpp")
target_include_directories(pathosub PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(pathosub PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# proxy
add_executable(lvcache "lvcache.cpp")
target_include_directories(lvcache PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(lvcache PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})