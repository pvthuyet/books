# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# taskwork
add_executable(taskwork2 taskwork2.cpp)
target_include_directories(taskwork2 PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(taskwork2 PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# tasksink
add_executable(tasksink2 "tasksink2.cpp")
target_include_directories(tasksink2 PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(tasksink2 PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})