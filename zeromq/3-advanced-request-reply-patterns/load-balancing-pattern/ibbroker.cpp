#include <Windows.h>
#include <zhelpers.hpp>
#include <thread>
#include <syncstream>
#include <sstream>
#include <fmt/format.h>
#include <queue>
#include <gsl/gsl_assert>
#include <sstream>

using namespace std::string_literals;
using namespace std::string_view_literals;

const std::string FE_ADDR = "tcp://*:5560";
const std::string BE_ADDR = "tcp://*:5559";

inline std::string current_thread_id()
{
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

void client_thread(std::stop_token stk, int id)
{
    try {
        zmq::context_t ctx(1);
        zmq::socket_t client(ctx, ZMQ_REQ);
#if (defined (WIN32))
        s_set_id(client, (intptr_t)id);
        client.connect("tcp://localhost:5560"); // frontend
#else
        s_set_id(client); // Set a printable identity
        client.connect("ipc://frontend.ipc");
#endif

        // send request, get reply
        std::string msg = "HELLO";
        //std::cout << fmt::format("[{}] {}\n", current_thread_id(), msg);
        s_send(client, msg);
        auto reply = s_recv(client);
        std::cout << std::format("[client {}] {} => {}\n", client.get(zmq::sockopt::routing_id), msg, reply);
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void worker_thread(std::stop_token stk, int id)
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    zmq::context_t ctx(1);
    zmq::socket_t worker(ctx, ZMQ_REQ);

#if (defined (WIN32))
    s_set_id(worker, id);
    worker.connect("tcp://localhost:5559"); // backend
#else
    s_set_id(worker);
    worker.connect("ipc://backend.ipc");
#endif

    // tell backend we're ready for work
    s_send(worker, "READY"s);

    while (1) {
        auto addr = s_recv(worker);
        {
            auto empty = s_recv(worker);
            Expects(empty.empty());
        }

        auto req = s_recv(worker);
        s_sendmore(worker, addr);
        s_sendmore(worker, "");
        s_send(worker, "OK"s);
        //std::cout << std::format("[worker {}] {} => {}\n", worker.get(zmq::sockopt::routing_id), req, "OK");
    }
}

int main()
{
    try {
        zmq::context_t ctx(1);
        zmq::socket_t frontend(ctx, ZMQ_ROUTER);
        zmq::socket_t backend(ctx, ZMQ_ROUTER);

#if (defined (WIN32))
        frontend.bind(FE_ADDR); // frontend
        backend.bind(BE_ADDR); // backend
#else
        frontend.bind("ipc://frontend.ipc");
        backend.bind("ipc://backend.ipc");
#endif

        constexpr const int CL_NUM = 10;
        std::vector<std::jthread> cl_pool;
        cl_pool.reserve(CL_NUM);
        for (int i = 1; i <= CL_NUM; ++i) {
            cl_pool.push_back(std::jthread(client_thread, i));
        }

        constexpr const int WK_NUM = 3;
        std::vector<std::jthread> wk_pool;
        wk_pool.reserve(WK_NUM);
        for (int i = 1; i <= CL_NUM; ++i) {
            wk_pool.push_back(std::jthread(worker_thread, i));
        }

        int numActive = CL_NUM;
        std::queue<std::string> worker_queue;

        while (1) {

            zmq::pollitem_t items[] = {
{static_cast<void*>(backend), 0, ZMQ_POLLIN, 0 },
{static_cast<void*>(frontend), 0, ZMQ_POLLIN, 0 }
            };

            if (worker_queue.size())
                zmq::poll(&items[0], 2, -1);
            else
                zmq::poll(&items[0], 1, -1);

            // handle worker activity on backend
            if (items[0].revents & ZMQ_POLLIN) {

                // queue worker address fro lru routing
                worker_queue.push(s_recv(backend));
                {
                    // second frame is empty
                    auto empty = s_recv(backend);
                    Expects(empty.empty());
                }

                // Third frame is READY or else a client reply address
                auto cl_addr = s_recv(backend);

                // if client reply, send rest back to frontend
                if (cl_addr != "READY"s) {
                    {
                        auto empty = s_recv(backend);
                        Expects(empty.empty());
                    }
                    auto reply = s_recv(backend);
                    s_sendmore(frontend, cl_addr);
                    s_sendmore(frontend, "");
                    s_send(frontend, reply);

                    if (--numActive == 0) break;
                }
            }
            if (items[1].revents & ZMQ_POLLIN) {
                // client request
                auto cl_addr = s_recv(frontend);
                {
                    auto empty = s_recv(frontend);
                    Expects(empty.empty());
                }

                auto req = s_recv(frontend);

                auto worker_addrs = worker_queue.front();
                worker_queue.pop();

                s_sendmore(backend, worker_addrs);
                s_sendmore(backend, "");
                s_sendmore(backend, cl_addr);
                s_sendmore(backend, "");
                s_send(backend, req);
            }
        }
    }
    catch (std::exception const& ex) {
        std::cerr << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}