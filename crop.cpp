#include <deque>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include "simple/file.hpp"
#include "simple/support/enum.hpp"

using namespace simple;
using namespace std::literals;
using std::string;

enum class Options
{
	Padding,
	Color,
	Source,
	Format,
	Invalid
};
using Option = support::MappedEnum<Options, Options::Invalid, 2>;
template <> Option::Guts::map_type Option::Guts::map
{{
	{ "-p"s, "--padding"s },
	{ "-c"s, "--color"s },
	{ "-i"s, "--source"s },
	{ "-f"s, "--format"s },
}};

enum class Formats
{
	PlainText,
	OneLine,
	Hex,
	Invalid
};
using Format = support::MappedEnum<Formats, Formats::Invalid>;
template <> Format::Guts::map_type Format::Guts::map
{{
	{ "plain"s },
	{ "one-line"s },
	{ "hex"s },
}};

enum class Colors
{
	Black,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
	Invalid
};
using Color = support::MappedEnum<Colors, Colors::Invalid>;
template <> Color::Guts::map_type Color::Guts::map
{{
	{ "black"s },
	{ "red"s },
	{ "green"s },
	{ "yellow"s },
	{ "blue"s },
	{ "magenta"s },
	{ "cyan"s },
	{ "white"s }
}};

std::pair<size_t,size_t> stoull_pair(const std::string& str)
{
	size_t separator;
	auto first = std::stoull(str, &separator, 0);
	auto second = std::stoull(str.substr(separator+1), nullptr, 0);
	if(':' == str[separator])
		second += first;
	return {first, second};
}

template <typename Iterator>
struct iterator_range
{
	using iterator_type = Iterator;
	using array_type = std::array<iterator_type, 2>;
	iterator_type& begin() { return raw[0]; }
	iterator_type& end() { return raw[1]; }
	const iterator_type& begin() const { return raw[0]; }
	const iterator_type& end() const { return raw[1]; }

	array_type raw;
};
template <typename Iterator>
iterator_range<Iterator> make_iterator_range(Iterator begin, Iterator end)
{
	return {begin, end};
}
using const_range = iterator_range<file::buffer<>::const_iterator>;

void print(const_range range, Format format, Color color = Colors::Invalid)
{
	auto cout_flags = std::cout.flags();
	std::cout << std::hex;
	if(Colors::Invalid != color)
		std::cout << "\33[4" << char('0' + (int)Colors(color)) << 'm';
	switch(format)
	{
		case Formats::Hex:
			for(auto&& c : range)
			{
				if(c <= 0xf)
					std::cout << '0';
				std::cout << +c;
			}
		break;
		case Formats::OneLine:
			for(auto&& c : range)
			{
				auto cc = '\n' == c ? 'n' : '\r' == c ? 'r' : c;
				if(cc != c)
					std::cout << "\33[7m" << cc << "\33[27m";
				else
					std::cout << c;
			}
		break;
		default:
			for(auto&& c : range)
				std::cout << c;
	}
	if(Colors::Invalid != color)
		std::cout << "\33[0m";
	std::cout.flags(cout_flags);
}

void crop(const string& filename, const string& range, Color color, size_t padding, Format format)
{
	auto p = stoull_pair(range);
	auto source = file::dump(file::bropex(filename));
	auto lower = std::min(source.begin() + p.first, source.end());
	auto upper = std::min(source.begin() + p.second, source.end());
	auto left = lower - std::min(p.first, padding);
	auto right = upper + std::min<size_t>(source.end() - upper, padding);
	print({left , lower}, format);
	print({lower, upper}, format, color);
	print({upper, right}, format);
}

void process_arguments(std::deque<string> args)
{
	string filename;
	Color color;
	Format format;
	size_t padding = 0;
	args.pop_front();
	while(!args.empty())
	{
		switch(Option(args.front()))
		{

			case Options::Padding:
				args.pop_front();
				padding = stoull(args.at(0), nullptr, 0);
			break;

			case Options::Color:
				args.pop_front();
				color = Color(args.at(0));
			break;

			case Options::Source:
				args.pop_front();
				filename = args.at(0);
			break;

			case Options::Format:
				args.pop_front();
				format = Format(args.at(0));
			break;

			default:
				crop(filename, args.front(), color, padding, format);
				std::cout << '\n';
			break;

		}
		args.pop_front();
	}
}

int main(int argc, char const* argv[]) try
{
	process_arguments({argv, argv + argc});
	return 0;
}
catch(...)
{
	if(errno)
		std::cout << "Oh nooo! " << std::strerror(errno) << '\n';
	throw;
}
