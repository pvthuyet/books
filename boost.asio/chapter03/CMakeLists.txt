add_executable(chapter03 main.cpp SyncTCPClient.hpp SyncTCPClient.cpp "SyncTCPServer.hpp" "SyncTCPServer.cpp" "SyncUDPClient.hpp" "SyncUDFClient.cpp" "TcpSection.hpp" "AsyncTcpClient.hpp" "AsyncTcpClient.cpp")
target_include_directories(chapter03 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(chapter03 PRIVATE ${Boost_LIBRARIES})