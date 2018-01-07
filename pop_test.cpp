#include "simple/file.hpp"
#include <string>
#include <iostream>

using namespace std;
using namespace simple;
using file::string_stack::pop;
using file::string_stack::push;

int main()
{
	string one, two;
	string command;
	while(cin >> one)
	{
		cout << one << "\n\n";
		while(cin >> command)
		{
			if("pop" == command)
				push( two, pop(one) );
			else if("push" == command)
				push( one, pop(two) );
			cout << "one: " << one << '\n';
			cout << "two: " << two << "\n\n";
		}
		two.clear();
		cin.ignore();
		cin.clear();
	}
	return 0;
}
