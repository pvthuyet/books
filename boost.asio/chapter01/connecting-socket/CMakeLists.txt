add_executable(connecting-socket "main.cpp")
target_include_directories(connecting-socket PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(connecting-socket PRIVATE ${Boost_LIBRARIES})