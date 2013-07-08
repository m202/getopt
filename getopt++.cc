#include <getopt++.h>
using m202::GetOpt;
using m202::Option;
using std::function;
using std::string;
using std::vector;
#include <cstring>
using std::memset;
using std::memcpy;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <map>
using std::map;

GetOpt::GetOpt(int count, char **vect): argc(count), argv(vect), _short_opts(nullptr), _long_opts(nullptr) {
	error_func = default_error_handler;
}

GetOpt::~GetOpt() {
	for(Option *opt: long_opts) delete opt;
	for(Option *opt: short_opts) delete opt;
}

void GetOpt::add_option(char name, option_func_t func, bool has_argument, bool required) {
	char str[2];
	str[0] = name;
	str[1] = '\0';
	Option *opt = new Option(str, func, has_argument, required);
	short_opts.push_back(opt);
}

void GetOpt::add_option(const char *name, option_func_t func, bool has_argument, bool required) {
	Option *opt = new Option(name, func, has_argument, required);
	long_opts.push_back(opt);
}

void GetOpt::add_option(string &name, option_func_t func, bool has_argument, bool required) {
	Option *opt = new Option(name, func, has_argument, required);
	long_opts.push_back(opt);
}

void GetOpt::set_error_handler(option_error_func_t func) {
	opterr = func == nullptr? 1: 0;
	error_func = func;
}

int GetOpt::get_argc() const {
	return argc;
}

char **GetOpt::get_argv() const {
	return argv;
}

int GetOpt::run() {
	int opt = 0;
	int option_index = 0;
	char name_str[2] = "\0";

	optarg = 0;
	optind = 1;

	map<char, option_func_t> short_funcs;
	map<char*, option_func_t> long_funcs;

	build_short_opts();
	build_long_opts();

	for(Option *s: short_opts) {
		short_funcs[s->option_element.name[0]] = s->option_func;
	}

	for(Option *s: long_opts) {
		long_funcs[const_cast<char*>(s->option_element.name)] = s->option_func;
	}

	while((opt = getopt_long(argc, argv, _short_opts, _long_opts, &option_index)) != -1) {
		if(opt == '?') {
			if(error_func != nullptr)
				error_func(*this);

			break;
		}
		if(opt == 0) {
			char *name = const_cast<char*>(_long_opts[option_index].name);
			option_func_t func = long_funcs[name];

			if(func != nullptr) {
				if(optarg == nullptr)
					func(string(name), string(""), *this);
				else
					func(string(name), string(optarg), *this);
			}
		} else if(opt != -1) {
			char name = (char) opt;
			option_func_t func = short_funcs[name];

			name_str[0] = name;
			if(func != nullptr) {
				if(optarg == nullptr)
					func(string(name_str), string(""), *this);
				else
					func(string(name_str), string(optarg), *this);
			}
		}
		option_index = 0;
	}

	free_short_opts();
	free_long_opts();
	return optind;
}

#ifdef GETOPTPP_DEBUG
void GetOpt::debug() {
	cout << "Short options:" << endl;
	for(Option *s: short_opts) {
		cout << "- " << s->option_element.name;
		if(s->option_element.has_arg == required_argument)
			cout << " [R]";
		else if(s->option_element.has_arg == optional_argument)
			cout << " [O]";
		cout << endl;
	}
	build_short_opts();
	cout << "Short option string=\"" << _short_opts << '"' << endl;

	cout << "Long options:" << endl;
	for(Option *s: long_opts) {
		cout << "- " << s->option_element.name;
		if(s->option_element.has_arg == required_argument)
			cout << " [R]";
		else if(s->option_element.has_arg == optional_argument)
			cout << " [O]";
		cout << endl;
	}
	free_short_opts();
}
#endif /* GETOPTPP_DEBUG */

void GetOpt::build_short_opts() {
	free_short_opts();
	size_t len = short_opts.size() * 3 + 1;
	size_t index = 0;
	_short_opts = new char[len];
	memset(_short_opts, '\0', len);

	for(Option *opt: short_opts) {
		_short_opts[index++] = opt->option_element.name[0];
		if(opt->option_element.has_arg == required_argument || opt->option_element.has_arg == optional_argument)
			_short_opts[index++] = ':';
		if(opt->option_element.has_arg == optional_argument)
			_short_opts[index++] = ':';
	}
}

void GetOpt::free_short_opts() {
	if(_short_opts != nullptr) {
		delete [] _short_opts;
		_short_opts = nullptr;
	}
}

void GetOpt::build_long_opts() {
	free_long_opts();
	size_t len = long_opts.size() + 1;
	size_t index = 0;
	_long_opts = new struct option[len];

	_long_opts[len - 1] = null_option;

	for(Option *opt: long_opts) {
		_long_opts[index++] = opt->option_element;
	}
}

void GetOpt::free_long_opts() {
	if(_long_opts != nullptr) {
		delete[] _long_opts;
		_long_opts = nullptr;
	}
}

void GetOpt::default_error_handler(const GetOpt &getopt) {
	cerr << "Usage:\t" << getopt.get_argv()[0];

	for(Option *o: getopt.short_opts) {
		cerr << " [-" << o->option_element.name;
		if(o->option_element.has_arg == optional_argument) {
			cerr << "[<arg>]";
		} else if(o->option_element.has_arg == required_argument) {
			cerr << "<arg>";
		}
		cerr << "]";
	}

	for(Option *o: getopt.long_opts) {
		cerr << " [--" << o->option_element.name;
		if(o->option_element.has_arg == optional_argument) {
			cerr << "[=<arg>]";
		} else if(o->option_element.has_arg == required_argument) {
			cerr << " <arg>";
		}
		cerr << "]";
	}

	cerr << " ..." << endl;
	exit(0);
}

Option::Option(string &name, GetOpt::option_func_t func, bool has_argument, bool required):
	Option(name.c_str(), func, has_argument, required) { }

Option::Option(const char *name, GetOpt::option_func_t func, bool has_argument, bool required) {
	int len = strlen(name);
	char *name_c = new char[len + 1];

	opterr = 1;
	optopt = 0;

	if(name_c != nullptr) {
		memcpy(name_c, name, len + 1);
		if(has_argument || required) {
			if(required) option_element.has_arg = required_argument;
			else option_element.has_arg = optional_argument;
		}
		else option_element.has_arg = no_argument;
		option_element.name = const_cast<char*>(name_c);
		option_func = func;
	}
}


Option::~Option() {
	if(option_element.name != nullptr) {
		delete [] option_element.name;
	}
}
