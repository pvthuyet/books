# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(clonesrv1 "clonesrv1.cpp" "kvsimple.hpp")
target_include_directories(clonesrv1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonesrv1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client
add_executable(clonecli1 "clonecli1.cpp" "kvsimple.hpp" )
target_include_directories(clonecli1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonecli1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})