#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

namespace ipc = boost::interprocess;
int main()
{
	ipc::file_mapping fm("D:\\projects\\books\\boost.asio\\boost-good-example\\fastest-way-read-file\\bigfile.txt", ipc::read_only);
	ipc::mapped_region region(fm, ipc::read_only, 0, 0);
	const char* begin = static_cast<const char*> (region.get_address());
	auto size = region.get_size();
	std::cout.write(begin, size);
	return EXIT_SUCCESS;
}