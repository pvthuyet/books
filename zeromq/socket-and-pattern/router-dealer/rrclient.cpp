#include <Windows.h>
#include <zhelpers.hpp>
#include <thread>
#include <sstream>

int main(int argc, char* argv[])
{
	zmq::context_t ctx(1);
	zmq::socket_t requester(ctx, ZMQ_REQ);
	requester.connect("tcp://localhost:5559");
	std::string clientid = "client " + std::string(argv[1]) + " ";
	for (int i = 0; i < 1000; ++i) {
		s_send(requester, clientid + std::to_string(i) + std::string(" hello"));
		std::string str = s_recv(requester);
		std::cout << "Received reply " << i << ": " << str << std::endl;
		s_sleep(1000);
	}
	return 0;
}