#include <deque>
#include <iostream>
#include <iomanip>
#include <string>
#include "simple/file.hpp"
#include "simple/support/enum.hpp"
#include "simple/support/misc.hpp"
#include "simple/support/algorithm.hpp"

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

using const_range = support::range<file::buffer<>::const_iterator>;

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
	auto r = support::storn<size_t>(range);
	const auto source = file::dump(file::bropex(filename));
	print(support::get_iterator_range(source, {r.lower()-padding , r.lower()}), format);
	print(support::get_iterator_range(source, r), format, color);
	print(support::get_iterator_range(source, {r.upper(), r.upper()+padding}), format);
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
				padding = support::ston<size_t>(args.at(0));
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
		std::perror("Oh nooo!");
	throw;
}
