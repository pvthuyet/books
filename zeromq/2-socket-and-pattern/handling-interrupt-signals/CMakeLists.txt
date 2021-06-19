# using static library of zero mq
add_compile_definitions(ZMQ_STATIC) ## or #define ZMQ_STATIC beofre include zmq.hpp

# taskwork
add_executable(taskwork2 taskwork2.cpp)
target_include_directories(taskwork2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(taskwork2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})

# tasksink
add_executable(tasksink2 "tasksink2.cpp")
target_include_directories(tasksink2 PRIVATE ${SGLIB_INCLUDEDIR})
target_link_libraries(tasksink2 PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})