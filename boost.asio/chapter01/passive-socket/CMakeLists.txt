add_executable(passive-socket main.cpp)
target_include_directories(passive-socket PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(passive-socket PRIVATE ${Boost_LIBRARIES})
