#include <zmq.hpp>
#include <thread>
#include <iostream>

int main()
{
    // Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv(&request);
        std::cout << "Received Hello" << std::endl;

        //  Do some 'work'
        std::this_thread::sleep_for(std::chrono::seconds(3));

        //  Send reply back to client
        zmq::message_t reply(5);
        memcpy(reply.data(), "World", 5);
        socket.send(reply);
    }
	return EXIT_SUCCESS;
}