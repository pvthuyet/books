# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# taskevent
add_executable(taskevent taskevent.cpp)
target_include_directories(taskevent PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(taskevent PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# taskwork
add_executable(taskwork taskwork.cpp)
target_include_directories(taskwork PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(taskwork PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# taskwork
add_executable(tasksink tasksink.cpp)
target_include_directories(tasksink PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(tasksink PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})