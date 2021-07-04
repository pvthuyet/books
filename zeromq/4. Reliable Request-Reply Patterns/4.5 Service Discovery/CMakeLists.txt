# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(mmiecho "mmiecho.cpp" "mdp.hpp")
target_include_directories(mmiecho PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mmiecho PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})