# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp
# add_compile_definitions(_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH)


# server
add_executable(psenvpub psenvpub.cpp)
target_include_directories(psenvpub PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(psenvpub PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client
add_executable(psenvsub psenvsub.cpp)
target_include_directories(psenvsub PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(psenvsub PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})