#include <cstdio>
#include <algorithm>
#include "simple/file.hpp"
#include "simple/geom/vector.hpp"
#include "simple/geom/segment.hpp"
#include "simple/support/arithmetic.hpp"
#include "simple/support/misc.hpp"

using namespace simple::file;
using index_type = simple::geom::vector<size_t, 2>;
using double_buffer = std::pair<buffer<>, buffer<>>;

index_type get_size(const double_buffer& buffers) // NOTE: can be generalized to N dimensions
{
	return index_type(buffers.first.size(), buffers.second.size());
}

bool diff_at(const double_buffer& buffers, index_type position) // NOTE: can be generalized to N dimensions
{
	return buffers.first[position.x()] != buffers.second[position.y()];
}

index_type find_change(const double_buffer& buffers, index_type start = index_type::zero()) // NOTE: this is almost std::find_if, except the != bound comparison is not sufficient in this case... such a shame... i must be missing something...
{
	auto size = get_size(buffers);
	while(start < size)
	{
		if(diff_at(buffers, start))
			break;
		++start;
	}
	return start;
}

index_type measure_change(const double_buffer& buffers, index_type start) // NOTE: could be generalized to N dimensions... not trivial and no obvious gain
{
	auto distance = get_size(buffers) - start;

	auto minmax = std::minmax_element(distance.begin(), distance.end());
	auto min_index = index_type::unit(minmax.first - distance.begin());
	auto max_index = index_type::unit(minmax.second - distance.begin());

	size_t change_size = 1;
	auto change = max_index * change_size;
	auto step = - max_index + min_index;
	while(change < distance)
	{
		do
		{
			if(!diff_at(buffers, start + change))
				return change;
			change += step;
		}
		while(change < distance);

		++change_size;
		size_t excess; // NOTE: don't you hate it when edge cases are not cleanly handled by the main loop?... must figure this out...
		if(simple::support::sub_overflow(excess, change_size, *minmax.second - 1))
			excess = 0;
		change = max_index * (change_size - excess) + min_index * excess;
	}
	return distance;
}

void showChange(index_type position, index_type change)
{
	using simple::support::to_string;
	std::puts("--------");
	for(size_t i = 0; i < index_type::dimensions; ++i)
		std::puts( to_string<index_type::value_type>(simple::geom::segment<size_t>{position[i], change[i]}, ':').c_str() );
}

int main(int argc, char const* argv[]) try
{
	if(argc <= 2)
	{
		std::fputs("Need 2 parameters!", stderr);
		return -1;
	}

	const auto buffers = std::make_pair( dump(bropex(argv[1])), dump(bropex(argv[2])) );
	auto size = get_size(buffers);
	auto it = index_type::zero();
	while(it = find_change(buffers, it), it < size)
	{
		auto change = measure_change(buffers, it);
		showChange(it, change);
		it += change;
	}

	auto change = size - it;
	if(index_type::zero() != change)
		showChange(it, change);

	return 0;
}
catch(...)
{
	if(errno) std::perror("Oh nooo!");
	throw;
}
