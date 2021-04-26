add_executable(socket-enpoint main.cpp)
target_include_directories(socket-enpoint PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(socket-enpoint PRIVATE ${Boost_LIBRARIES})