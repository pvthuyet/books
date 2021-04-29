add_executable(connecting-socket2 "main.cpp")
target_include_directories(connecting-socket2 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(connecting-socket2 PRIVATE ${Boost_LIBRARIES})