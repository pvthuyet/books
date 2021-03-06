//
//  Weather update client in C++
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode
//

#include <zmq.hpp>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
    std::string addr = "tcp://localhost:5556";
    if (argc > 2) {
        addr = "tcp://localhost:" + std::string{ argv[1] };
    }
    zmq::context_t context(1);

    //  Socket to talk to server
    std::cout << "Collecting updates from weather server...\n" << std::endl;
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect(addr);

    //  Subscribe to zipcode, default is NYC, 10001
    const char* filter = (argc > 2) ? argv[2] : "10001 ";
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

    //  Process 100 updates
    int num = 0;
    while (true) {
        int update_nbr;
        long total_temp = 0;
        for (update_nbr = 0; update_nbr < 2; update_nbr++) {

            zmq::message_t update;
            int zipcode, temperature, relhumidity;

            subscriber.recv(&update);

            std::istringstream iss(static_cast<char*>(update.data()));
            iss >> zipcode >> temperature >> relhumidity;

            total_temp += temperature;
        }
        std::cout << "Average temperature for zipcode '" << filter
            << "' was " << (int)(total_temp / update_nbr) << "F"
            << std::endl;
    }
    return 0;
}