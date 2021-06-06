#include <iostream>
#include "service.hpp"
#include "sync_parallel_tcp.hpp"
#include "async_server.hpp"

void sync_server_test()
{
	server_demo ser;
	ser.start(3333);
	std::cout << "Press ENTER to exit.\n";
	std::cin.get();
	ser.stop();
}

void par_server_test()
{
	par_server::start_server();
}

void async_server_test()
{
	async_server::start_server();
}

int main()
{
	try {
		//par_server_test();
		async_server_test();
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	return EXIT_SUCCESS;
}