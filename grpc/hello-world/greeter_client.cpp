#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

class GreeterClient
{
private:
	std::unique_ptr<Greeter::Stub> stub_;

public:
	GreeterClient(std::shared_ptr<Channel> channel) 
		: stub_{Greeter::NewStub(channel)}
	{}

	std::string SayHello(const std::string& user)
	{
		// Data we are sending to the server
		HelloRequest request;
		request.set_name(user);

		// Container for the data we expect from the server
		HelloReply reply;

		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors
		ClientContext context;

		// The actual RPC
		Status status = stub_->SayHello(&context, request, &reply);

		// Acgt upon its status
		if (status.ok()) {
			return reply.message();
		}
		else {
			std::cout << status.error_code() << ": " << status.error_message() << std::endl;
			return "RPC failed";
		}
	}

	std::string SayHelloAgain(const std::string& user)
	{
		HelloRequest request;
		request.set_name(user);
		HelloReply reply;
		ClientContext ctx;

		auto status = stub_->SayHelloAgain(&ctx, request, &reply);
		if (status.ok()) {
			return reply.message();
		}
		else {
			std::cout << status.error_code() << ": " << status.error_message() << std::endl;
			return "RPC failed";
		}
	}
};

int main(int argc, char** argv)
{
	// Instantiate the client. It requires a channel, out of which the actual RPCs
	// are created. This channel models a connection to an endpoint specified by
	// the argument "--target=" which is the only expected argument.
	// We indicate that the channel isn't authenticated (use of
	// InsecureChannelCredentials()).
	std::string target_str = "localhost:50051";

	GreeterClient greeter(
		grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials())
	);

	std::string user = std::format("world");
	std::string reply = greeter.SayHello(user);
	std::cout << "Greeter received: " << reply << std::endl;

	reply = greeter.SayHelloAgain(user);
	std::cout << "Greeter received: " << reply << std::endl;

	return EXIT_SUCCESS;
}