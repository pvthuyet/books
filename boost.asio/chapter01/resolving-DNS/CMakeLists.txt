add_executable(resolving-DNS main.cpp)
target_include_directories(resolving-DNS PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(resolving-DNS PRIVATE ${Boost_LIBRARIES})