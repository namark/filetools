#include <iostream>
#include <algorithm>
#include <cstring>
#include <array>
#include "simple/file.hpp"
#include "simple/geom/vector.hpp"

using namespace simple;
using index_type = geom::vector<size_t, 2>;

int main(int argc, char const* argv[]) try
{
	switch(argc)
	{
		case 0: std::cerr << "What even is?!\n";
		case 1: std::cerr << "Filename not specified!\n";
		case 2: std::cerr << "Search string not specified!\n";
				return -1;
	}

	const auto buffer = file::dump(file::bropex(argv[1]));
	const char* target = argv[2];

	std::array<const char*, 2> data { buffer.data(), target };
	const index_type size(buffer.size(), std::strlen(target));

	constexpr auto hay = 0;
	constexpr auto needle = 1;
	constexpr auto hay_mask = index_type::unit<hay>();
	constexpr auto needle_mask = index_type::unit<needle>();
	auto i = index_type::zero();
	while(i < size)
	{
		if(data[hay][i[hay]] == data[needle][i[needle]])
			++i;
		else
			++i *= hay_mask;

		if( (i - size) * needle_mask == index_type::zero() )
		{
			const auto needle_size = size[needle];
			const auto needle_start = i[hay]-needle_size;
			std::cout << needle_start << ':' << needle_size  << '\n';
			i *= hay_mask;
		}
	}

	return 0;
}
catch(...)
{
	if(errno) std::perror("Oh nooo!");
	throw;
}
