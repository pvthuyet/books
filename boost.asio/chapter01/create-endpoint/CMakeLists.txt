# Client app
add_executable(client "client.cpp")
target_include_directories(client PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(client PRIVATE 
	Boost::date_time
	Boost::regex
	)


#Server app
add_executable(server "server.cpp")
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(server PRIVATE 
	Boost::date_time
	Boost::regex
	)
