# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(mdclient2 "mdcliapi2.hpp" "mdclient2.cpp" "mdp.hpp")
target_include_directories(mdclient2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mdclient2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})