add_executable(client-server main.cpp "echoserver.hpp" "echoserver.cpp" "define.hpp" 
"hive.hpp" "hive.cpp" 
"acceptor.hpp"
"acceptor.cpp"
 "connection.hpp" "connection.cpp" "echo_server_connection.cpp"
 "echo_server_connection.hpp"
 "echo_server_connection.cpp"
  "logger.hpp"  "echo_server_acceptor.hpp" "echo_server_acceptor.cpp")
target_include_directories(client-server PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(client-server PRIVATE ${Boost_LIBRARIES})