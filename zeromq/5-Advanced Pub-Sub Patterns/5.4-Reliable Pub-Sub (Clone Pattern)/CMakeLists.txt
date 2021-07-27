# using static library of zero mq
add_compile_definitions(ZMQ_STATIC ZMQPP_STATIC_DEFINE) ## or #define ZMQ_STATIC beofre include zmq.hpp

# server
add_executable(clonesrv1 "clonesrv1.cpp" "kvsimple.hpp")
target_include_directories(clonesrv1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonesrv1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client
add_executable(clonecli1 "clonecli1.cpp" "kvsimple.hpp")
target_include_directories(clonecli1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonecli1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

## Getting an Out-of-Band Snapshot
# server
add_executable(clonesrv2 "clonesrv2.cpp" "kvsimple.hpp")
target_include_directories(clonesrv2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonesrv2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# client
add_executable(clonecli2 "clonecli2.cpp" "kvsimple.hpp")
target_include_directories(clonecli2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonecli2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

## Republishing Updates from Clients
# server
add_executable(clonesrv3 "clonesrv3.cpp" "kvsimple.hpp")
target_include_directories(clonesrv3 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonesrv3 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})
#client
add_executable(clonecli3 "clonecli3.cpp" "kvsimple.hpp")
target_include_directories(clonecli3 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonecli3 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

## Working with Subtrees
# server
add_executable(clonesrv4 "clonesrv4.cpp" "kvsimple.hpp")
target_include_directories(clonesrv4 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonesrv4 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})
# client
add_executable(clonecli4 "clonecli4.cpp" "kvsimple.hpp")
target_include_directories(clonecli4 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonecli4 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

## Ephemeral Values
# client
add_executable(clonecli5 "clonecli5.cpp" "kvmsg.hpp")
target_include_directories(clonecli5 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonecli5 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})
# server
add_executable(clonesrv5 "clonesrv5.cpp" "kvmsg.hpp")
target_include_directories(clonesrv5 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonesrv5 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

## Adding the Binary Star Pattern for Reliability
# server
add_executable(clonesrv6 "clonesrv6.cpp" "kvmsg.hpp" "bstar.hpp")
target_include_directories(clonesrv6 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(clonesrv6 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})