# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# fileio1
add_executable(fileio1 "fileio1.cpp")
target_include_directories(fileio1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(fileio1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# fileio2
add_executable(fileio2 "fileio2.cpp")
target_include_directories(fileio2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(fileio2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})