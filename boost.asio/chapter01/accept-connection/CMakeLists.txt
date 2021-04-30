add_executable(accept-connection "main.cpp")
target_include_directories(accept-connection PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(accept-connection PRIVATE ${Boost_LIBRARIES})