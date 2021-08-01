#include "udplib.cpp"
#include <zmqpp/zmqpp.hpp>

//  UDP ping command
//  Model 2, uses separate UDP library
#define PING_PORT_NUMBER 9999
#define PING_MSG_SIZE    1
#define PING_INTERVAL    1000  //  Once per second

int main(void)
{
    zmqpp::context_t ctx;
    udp_t* udp = udp_new(PING_PORT_NUMBER);

    byte buffer[PING_MSG_SIZE];

    zmq_pollitem_t pollitems[] = {
        { NULL, udp_handle(udp), ZMQ_POLLIN, 0 }
    };
    //  Send first ping right away
    //uint64_t ping_at = zclock_time();

    auto ping_at = std::chrono::steady_clock::now();
    while (1) {
        //long timeout = (long)(ping_at - zclock_time());
        //if (timeout < 0)
        //    timeout = 0;

        if (zmq_poll(pollitems, 1, 1000) == -1)
            break;              //  Interrupted

        //  Someone answered our ping
        if (pollitems[0].revents & ZMQ_POLLIN)
            udp_recv(udp, buffer, PING_MSG_SIZE);

        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ping_at);
        if (diff.count() > PING_INTERVAL) {
            puts("Pinging peers...");
            buffer[0] = '!';
            udp_send(udp, buffer, PING_MSG_SIZE);
            ping_at = std::chrono::steady_clock::now();
        }
    }
    udp_destroy(&udp);
    //zctx_destroy(&ctx);
    return 0;
}