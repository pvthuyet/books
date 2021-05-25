#include <iostream>
#include "started_asio.hpp"
#include "tcp.hpp"
int main()
{
	//blocking_the_run1();
	//blocking_the_run2();
	//non_blocking_poll1();
	//non_blocking_poll2();
	//remove_work_object1();
	//dealing_many_thread1();
	//working_boost_bind();
	//using_post();
	//using_dispatch();
	//using_none_strand();
	//using_strand();
	//handling_exception();
	//synchronous_client();
	asynchronous_client();
	return EXIT_SUCCESS;
}