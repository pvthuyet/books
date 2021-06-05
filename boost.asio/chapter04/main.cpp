#include <iostream>
#include "service.hpp"

int main()
{
	try {
		server_demo ser;
		ser.start(3333);
		std::cout << "Press ENTER to exit.\n";
		std::cin.get();
		ser.stop();
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	return EXIT_SUCCESS;
}