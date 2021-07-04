# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(mdclient1  "mdcliapi.hpp" "mdclient.cpp" "mdp.h")
target_include_directories(mdclient1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mdclient1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server: broker
add_executable(mdbroker1 "mdbroker.cpp")
target_include_directories(mdbroker1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mdbroker1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server: worker
add_executable(mdworker1 "mdwrkapi.hpp" "mdworker.cpp")
target_include_directories(mdworker1 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mdworker1 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# tripping
add_executable(tripping "tripping.cpp")
target_include_directories(tripping PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(tripping PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})