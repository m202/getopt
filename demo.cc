#include <string>
using std::string;
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <getopt++.h>

using m202::GetOpt;

void fn(const string &option, const string &value, const GetOpt &getopt) {
	cerr << "fn: " << option << "=" << value << endl;
}

int main(int argc, char **argv) {
	int index = 0;
	m202::GetOpt o(argc, argv);

	// Assign options to function
	o.add_option('n', fn, false, false);
	o.add_option('o', fn, true, false);
	o.add_option('r', fn, true, true);
	o.add_option("none", fn, false, false);
	o.add_option("optional", fn, true, false);
	o.add_option("required", fn, true, true);

	cout << "Printing debug information:" << endl;
	o.debug();
	cout << endl;

	cout << "Before parse:" << endl;
	while(index < argc) {
		cout << "argv[" << index << "]=" << argv[index] << endl;
		index++;
	}
	cout << endl;

	cout << "Running parse:" << endl;
	index = o.run();
	cout << endl;

	cout << "After parse:" << endl;
	while(index < argc) {
		cout << "argv[" << index << "]=" << argv[index] << endl;
		index++;
	}

	return 0;
}
