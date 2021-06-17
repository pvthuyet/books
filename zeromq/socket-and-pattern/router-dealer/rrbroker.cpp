#include <Windows.h>
#include <zhelpers.hpp>

int main()
{
	zmq::context_t ctx(1);
	zmq::socket_t frontend(ctx, ZMQ_ROUTER);
	zmq::socket_t backend(ctx, ZMQ_DEALER);

	frontend.bind("tcp://*:5559");
	backend.bind("tcp://*:5560");

	zmq::pollitem_t item[] = {
		{static_cast<void*>(frontend), 0, ZMQ_POLLIN, 0},
		{static_cast<void*>(backend), 0, ZMQ_POLLIN, 0}
	};

	while (1) {
		zmq::message_t msg;
		int more;

		zmq::poll(&item[0], 2, -1);

		if (item[0].revents & ZMQ_POLLIN) {
			while (1) {
				//  Process all parts of the message
				frontend.recv(&msg);
				// frontend.recv(message, zmq::recv_flags::none); // new syntax
				size_t more_size = sizeof(more);
				frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
				backend.send(msg, more ? ZMQ_SNDMORE : 0);
				if (!more) break;
			}
		}

		if (item[1].revents & ZMQ_POLLIN) {
			while (1) {
				//  Process all parts of the message
				backend.recv(&msg);
				size_t more_size = sizeof more;
				backend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
				frontend.send(msg, more ? ZMQ_SNDMORE : 0);
				if (!more) break;
			}
		}
	}
	return 0;
}