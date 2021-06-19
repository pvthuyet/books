# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(msreader msreader.cpp)
target_include_directories(msreader PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(msreader PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

add_executable(mspoller mspoller.cpp)
target_include_directories(mspoller PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mspoller PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})