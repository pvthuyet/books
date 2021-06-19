# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(wuserver wuserver.cpp)
target_include_directories(wuserver PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(wuserver PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client
add_executable(wuclient wuclient.cpp)
target_include_directories(wuclient PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(wuclient PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# proxy
add_executable(wuproxy wuproxy.cpp)
target_include_directories(wuproxy PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(wuproxy PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})