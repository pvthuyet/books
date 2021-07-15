#include "publisher.hpp"
#include "subscriber.hpp"
#include "listener.hpp"
#include <zmqpp/proxy.hpp>

int main()
{
	zmqpp::context_t ctx{};

	// socket
	zmqpp::socket_t sock_pub(ctx, zmqpp::socket_type::xpub);
	sock_pub.bind("tcp://*:6001");

	publisher pub(ctx, "tcp://*:6000");

	zmqpp::socket_t sock_list(ctx, zmqpp::socket_type::pair);
	sock_list.bind("inproc://listener");

	zmqpp::socket_t sock_subs(ctx, zmqpp::socket_type::xsub);
	sock_subs.connect("tcp://127.0.0.1:6000");

	// object
	subscriber subs(ctx, "tcp://127.0.0.1:6001");
	listener list(ctx, "inproc://listener");

	subs.start();
	pub.start();
	list.start();

	zmqpp::proxy prxy(sock_subs, sock_pub, sock_list);
	
	return 0;
}