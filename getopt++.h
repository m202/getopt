#ifndef GETOPTPP_H
#define GETOPTPP_H

#include <functional>
#include <string>
#include <vector>

#include <unistd.h>
#include <getopt.h>

namespace m202 {
class Option;

class GetOpt {
	public:
		typedef std::function<void(const std::string &, const std::string &, const GetOpt &)> option_func_t;
		typedef std::function<void(const GetOpt &)> option_error_func_t;

		// Useless constructors deleted
		GetOpt() = delete;
		GetOpt(const GetOpt&) = delete;
		GetOpt& operator=(const GetOpt&) = delete;

		GetOpt(int argc, char **argv);
		~GetOpt();

		// Add short option callback
		void add_option(char name, option_func_t func, bool has_argument = false, bool required = false);

		// Add long option callback (C std::string)
		void add_option(const char *name, option_func_t func, bool has_argument = false, bool required = false);

		// Add long option callback (C++ std::string)
		void add_option(std::string &name, option_func_t func, bool has_argument = false, bool required = false);

		// Set callback for invalid options (also disables internal error message)
		void set_error_handler(option_error_func_t func);

		// Get argc and argv stored by object
		int get_argc() const;
		char **get_argv() const;

		// Process command line
		int run();

#ifdef GETOPTPP_DEBUG
		// Dump valid options to stdout
		void debug();
#endif /* GOPP_DEBUG */

	private:
		constexpr static struct option null_option {
			.name = nullptr,
				.has_arg = 0,
				.flag = nullptr,
				.val = 0
		};

		int argc;
		char **argv;
		std::vector<Option*> short_opts;
		std::vector<Option*> long_opts;
		option_error_func_t error_func;
		char *_short_opts;
		struct option *_long_opts;

		void build_short_opts();
		void free_short_opts();
		void build_long_opts();
		void free_long_opts();
		static void default_error_handler(const GetOpt &);
};

class Option {
	private:
		struct option option_element;
		GetOpt::option_func_t option_func;

		Option(std::string &name, GetOpt::option_func_t func, bool has_argument, bool required);
		Option(const char *name, GetOpt::option_func_t func, bool has_argument, bool required);
		~Option();

	friend class GetOpt;
		
};
}

#endif /* GETOPTPP_H */
