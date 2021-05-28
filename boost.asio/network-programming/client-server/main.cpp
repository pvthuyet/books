#include <iostream>
#include "define.hpp"
#include "echoserver.hpp"

int main()
{
	SG::echo_server{}.start();
	return EXIT_SUCCESS;
}