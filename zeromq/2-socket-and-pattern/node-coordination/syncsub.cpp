#include <Windows.h>
#include <zhelpers.hpp>

int main()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;
	zmq::context_t ctx(1);

	//  First, connect our subscriber socket
	zmq::socket_t subs(ctx, ZMQ_SUB);
	subs.connect("tcp://localhost:5561");
	subs.set(zmq::sockopt::subscribe, ""sv);

	//  Second, synchronize with publisher
	zmq::socket_t syncclient(ctx, ZMQ_REQ);
	syncclient.connect("tcp://localhost:5562");

	//  - send a synchronization request
	s_send(syncclient, ""s);

	//  - wait for synchronization reply
	s_recv(syncclient);

	//  Third, get our updates and report how many we got
	int update = 0;
	while (1) {
		auto msg = s_recv(subs);
		if (msg == "END") {
			break;
		}
		std::cout << msg << std::endl;
		update++;
	}
	std::cout << "Received " << update << " updates" << std::endl;
	return 0;
}