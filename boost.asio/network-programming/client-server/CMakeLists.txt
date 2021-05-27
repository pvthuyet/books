add_executable(client-server main.cpp "echoserver.hpp" "echoserver.cpp" "define.hpp" "hive.hpp" "hive.cpp")
target_include_directories(client-server PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(client-server PRIVATE ${Boost_LIBRARIES})