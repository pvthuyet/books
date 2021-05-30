#include <iostream>
#include "define.hpp"
#include "echoserver.hpp"
#include "client_get_http.hpp"

int main()
{
	//SG::echo_server{}.start();
	SG::client_get_http{}.start();
	return EXIT_SUCCESS;
}