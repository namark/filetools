#include <iostream>
#include "simple/file.hpp"

using namespace std;
using namespace simple;
using namespace file::operators;

void glue(string path)
{
	auto info = file::ropex(path);
	file::string_stack::pop(path);

	string name;
	getline(info, name);

	file::size_type expected_size;
	info >> expected_size;
	info.ignore();

	auto output = file::bwopex(path + name);

	string piece;
	while(info.peek()|1 && !info.eof() && getline(info, piece))
		output <<= file::bropex(path + piece);

	auto diff = file::size(output) - expected_size;
	if(diff)
		cerr << "Output size differs from expected by " << diff << '\n';
}

void process_arguments(int argc, char const * argv[])
{
	for(int i = 1; i < argc; ++i)
		glue(argv[i]);
}

void process_input()
{
	string target;
	while(getline(cin, target))
		glue(target);
}

int main(int argc, char const * argv[])
{
	if(argc < 2)
		process_input();
	else
		process_arguments(argc, argv);

	return 0;
}
