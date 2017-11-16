#include <iostream>
#include "simple/file.hpp"

using namespace simple;
using namespace simple::file::operators;

void swap(const std::string& onePath, const std::string& otherPath)
{
	if(onePath == otherPath)
		return;

	auto one = file::dump(file::bropex(onePath));
	auto other = file::dump(file::bropex(otherPath));

	file::bwopex(onePath) <<= other;
	file::bwopex(otherPath) <<= one;
}

int main(int argc, char const* argv[])
{
	if(argc < 3)
	{
		std::cout << "Expecting two arguments, got " << argc - 1 << '\n';
		return -1;
	}

	if(argc > 3)
		std::cout << "Ignoring all arguments after the first 2" << '\n';

	swap(argv[1], argv[2]);

	return 0;
}
