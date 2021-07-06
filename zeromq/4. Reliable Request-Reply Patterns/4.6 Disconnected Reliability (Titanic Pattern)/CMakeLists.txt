# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(ticlient "ticlient.cpp")
target_include_directories(ticlient PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(ticlient PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# titanic broker
add_executable(titanic "titanic.cpp")
target_include_directories(titanic PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(titanic PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})