# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# client
add_executable(mdclient  "mdcliapi.hpp" "mdclient.cpp" "mdp.hpp")
target_include_directories(mdclient PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mdclient PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server: broker
add_executable(mdbroker "mdbroker.cpp")
target_include_directories(mdbroker PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mdbroker PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# server: worker
add_executable(mdworker "mdwrkapi.hpp" "mdworker.cpp")
target_include_directories(mdworker PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(mdworker PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})