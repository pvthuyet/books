add_executable(chapter03 main.cpp SyncTCPClient.hpp SyncTCPClient.cpp "SyncTCPServer.hpp" "SyncTCPServer.cpp")
target_include_directories(chapter03 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(chapter03 PRIVATE ${Boost_LIBRARIES})