# active-socket
add_executable(active-socket main.cpp)
target_include_directories(active-socket PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(active-socket PRIVATE ${Boost_LIBRARIES})
