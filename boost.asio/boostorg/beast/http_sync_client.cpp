#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main(int argc, char** argv)
{
	try {
        // Check command line arguments.
        if (argc != 4 && argc != 5)
        {
            std::cerr <<
                "Usage: http-client-sync <host> <port> <target> [<HTTP version: 1.0 or 1.1(default)>]\n" <<
                "Example:\n" <<
                "    http-client-sync www.example.com 80 /\n" <<
                "    http-client-sync www.example.com 80 / 1.0\n";
            return EXIT_FAILURE;
        }

        auto const host = argv[1];
        auto const port = argv[2];
        auto const target = argv[3];
        int version = argc == 5 && !std::strcmp("1.0", argv[4]) ? 10 : 11;

        net::io_context ioc;

        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);

        stream.connect(results);

        http::request<http::string_body> req{ http::verb::get, target, version };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // send the http request to the remote host
        http::write(stream, req);

        // Receive the HTTP response
        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);

        std::cout << res << std::endl;

        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error{ ec };
        }
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
	}
    return EXIT_SUCCESS;
}