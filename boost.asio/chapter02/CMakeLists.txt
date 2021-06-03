add_executable(chapter02 main.cpp "sync_client.hpp" "sync_client.cpp" "sync_server.hpp" "sync_server.cpp")
target_include_directories(chapter02 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(chapter02 PRIVATE ${Boost_LIBRARIES})