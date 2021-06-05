add_executable(chapter04 main.cpp "service.hpp" "sync_parallel_tcp.hpp")
target_include_directories(chapter04 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(chapter04 PRIVATE ${Boost_LIBRARIES})