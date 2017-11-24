#include <iostream>
#include <sstream>
#include <string>
#include <deque>
#include <map>
#include <functional>
#include <algorithm>

#include "simple/file.hpp"
#include "simple/support/enum.hpp"

using namespace std;
using namespace std::literals;
using namespace simple;
using namespace file::operators;
using file::string_stack::pop;
using file::string_stack::drop;
using file::string_stack::push;
using string_stack = file::string_stack::manipulator<>;

static const string info_ext = "info"s;
static const string piece_ext = "piece"s;
static const string ext_delim = "."s;

static constexpr file::size_type default_piece_size = 256 * 1024;

struct _option_guts
{

	enum class Type
	{
		PieceSize,
		Terminator,
		Invalid
	};

	static constexpr Type Default = Type::Invalid;

	static Type from(string name)
	{
		static const map<string, Type> stringToOption
		{
			{ "-s"s, Type::PieceSize },
			{ "-f"s, Type::Terminator }
		};
		auto found = stringToOption.find(name);
		return found != stringToOption.end() ? found->second : Type::Invalid;
	};

};

using Option = support::Enum<_option_guts>;
using Options = Option::Type;

string to_hex_string(int n)
{
	stringstream s;
	s << hex << n;
	return s.str();
}

// why use bind when got lambdas? hmm?
using namespace placeholders;
auto popext = bind(pop<>, _1, ext_delim);
auto pushext = bind(push<>, _1, _2, ext_delim);
auto dropext = bind(drop<>, _1, ext_delim);

// // well there are your lambdas... like em?
// auto popext = [&](auto& _1){ return pop(_1, ext_delim); };
// auto pushext = [&](auto& _1, auto& _2){ push(_1, _2, ext_delim); };
// auto dropext = [&](auto& _1){ drop(_1, ext_delim); };

void shred(string path, file::size_type piece_size = default_piece_size)
{
	auto victim = file::bropex(path);
	auto victim_size = file::size(victim);

	string name = pop(path);
	string ext = popext(name); // TODO: separating the extension is ugly and problematic
	if(name.empty())
		swap(name, ext);

	push(path, name);
	pushext(path, info_ext);
	auto info = file::wopex(path);
	drop(path);

	{ auto extension = string_stack(name, ext_delim).push(ext);
		info << name << '\n'
			<< victim_size << '\n';
	}

	file::buffer<> piece;
	for(int i = 1; victim_size; ++i, victim_size -= piece.size())
	{

		auto name_stack = string_stack(name, ext_delim).push(to_hex_string(i), piece_ext);
		auto path_stack = string_stack(path).push(name);

		piece.resize(min(victim_size, piece_size));
		victim.read(piece.data(), piece.size());
		file::bwopex(path) <<= piece;
		info << name << '\n';

	}

}

void process_arguments(deque<string> args)
{
	args.pop_front();
	auto piece_size = default_piece_size;
	Option option;
	while(!args.empty())
	{
		option = Option(args.front());
		switch(option)
		{

			case Options::PieceSize:
				args.pop_front();
				if(args.empty())
					return;
				piece_size = stoull(args.front(), nullptr, 0);
			break;

			case Options::Terminator:
				args.pop_front();
				for(auto& arg : args)
					shred(arg, piece_size);
			return;

			default:
				shred(args.front(), piece_size);
			break;

		}
		args.pop_front();
	}
}

void process_input()
{
	auto piece_size = default_piece_size;
	string input;
	while(getline(cin, input))
	{
		piece_size = std::strtoull(input.c_str(), nullptr, 0);
		piece_size = piece_size ? piece_size : default_piece_size;
		while(getline(cin, input) && !input.empty())
			shred(input, piece_size);
	}
}

int main(int argc, char const * argv[])
{
	process_arguments( {argv, argv + argc} );
	if(argc < 2)
		process_input();

	return 0;
}
