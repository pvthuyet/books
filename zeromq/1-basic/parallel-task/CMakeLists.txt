# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# taskevent
add_executable(taskevent taskevent.cpp)
target_include_directories(taskevent PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(taskevent PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# taskwork
add_executable(taskwork taskwork.cpp)
target_include_directories(taskwork PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(taskwork PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})

# taskwork
add_executable(tasksink tasksink.cpp)
target_include_directories(tasksink PRIVATE ${ZERO_MQ_INCLUDEDIR} ${CPP_ZMQ_INCLUDEDIR})
target_link_libraries(tasksink PRIVATE ${ZERO_MQ_LIBRARIES} ${WINDOWS_LIBRARIES})