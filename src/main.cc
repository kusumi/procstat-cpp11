#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <string>
#include <exception>

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <csignal>

#include <getopt.h>

#include "./container.h"
#include "./global.h"
#include "./log.h"
#include "./screen.h"
#include "./thread.h"
#include "./watch.h"

extern char* optarg;
extern int optind;

volatile sig_atomic_t interrupted;

namespace opt {
	std::vector<int> layout;
	time_t sinterval;
	long ninterval;
	bool showlnum;
	bool foldline;
	bool blinkline = true;
	bool rotatecol;
	bool usedelay;
}

namespace {
const std::array<int, 3> _version{0, 2, 1};
std::vector<std::string> _what;

std::string get_version_string() {
	std::ostringstream ss;
	ss << _version[0] << "." << _version[1] << "." << _version[2];
	return ss.str();
}

void print_version() {
	std::cout << get_version_string() << std::endl;
}

void sigint_handler([[maybe_unused]] int n) {
	interrupted = 1;
}

void atexit_handler(void) {
	cleanup_screen();
	cleanup_watch();
	cleanup_log();
	cleanup_lock();
	for (const auto& s : _what)
		std::cerr << s << std::endl;
}

void print_build_options(void) {
	std::cout << "Build options:" << std::endl
#ifdef DEBUG
		<< "  debug" << std::endl
#endif
#ifdef CONFIG_STDOUT
		<< "  stdout" << std::endl
#endif
#ifdef CONFIG_STDTHREAD
		<< "  stdthread" << std::endl
#endif
#ifdef CONFIG_INOTIFY
		<< "  inotify" << std::endl
#endif
#ifdef CONFIG_GETTIMEOFDAY
		<< "  gettimeofday" << std::endl
#endif
		;
}

void usage(const std::string& arg) {
	std::cerr << "Usage: " << arg << " [options] /proc/..." << std::endl
		<< "Options:" << std::endl
		<< "  -c <arg> - Set column layout. "
		<< R"(e.g. "-c 123" to make 3 columns with 1,2,3 windows for each)"
		<< std::endl
		<< "  -t <arg> - Set refresh interval in second. Default is 1. "
		<< R"(e.g. "-t 5" to refresh screen every 5 seconds)"
		<< std::endl
		<< "  -m - Take refresh interval as milli second. "
		<< R"(e.g. "-t 500 -m" to refresh screen every 500 milli seconds)"
		<< std::endl
		<< "  -n - Show line number" << std::endl
		<< "  -f - Fold lines when longer than window width"
		<< std::endl
		<< "  -r - Rotate column layout" << std::endl
		<< "  -h - This option" << std::endl
		<< "  --fg <arg> - Set foreground color. Available colors are "
		<< R"("black", "blue", "cyan", "green", "magenta", "red", "white", "yellow".)"
		<< std::endl
		<< "  --bg <arg> - Set background color. Available colors are "
		<< R"("black", "blue", "cyan", "green", "magenta", "red", "white", "yellow".)"
		<< std::endl
		<< "  --noblink - Disable blink" << std::endl
		<< "  --usedelay - Add random delay time before each window starts"
		<< std::endl
		<< std::endl
		<< "Commands:" << std::endl
		<< "  0 - Set current position to the first line of the buffer"
		<< std::endl
		<< "  $ - Set current position to the last line of the buffer"
		<< std::endl
		<< "  k|UP - Scroll upward" << std::endl
		<< "  j|DOWN - Scroll downward" << std::endl
		<< "  h|LEFT - Select next window" << std::endl
		<< "  l|RIGHT - Select previous window" << std::endl
		<< "  CTRL-b - Scroll one page upward" << std::endl
		<< "  CTRL-u - Scroll half page upward" << std::endl
		<< "  CTRL-f - Scroll one page downward" << std::endl
		<< "  CTRL-d - Scroll half page downward" << std::endl
		<< "  CTRL-l - Repaint whole screen" << std::endl;
}
} // namespace

void add_exception(const std::exception &e) {
	global_lock();
	_what.push_back(e.what());
	global_unlock();
}

int main(int argc, char** argv) {
	const char* progname_ptr = argv[0];
	std::string progname(progname_ptr);

	int i, c;
	option lo[] = {
		{ "fg", 1, nullptr, 'F' },
		{ "bg", 1, nullptr, 'B' },
		{ "noblink", 0, nullptr, 'N' },
		{ "usedelay", 0, nullptr, 'T' },
		{ nullptr, 0, nullptr, 0 },
	};

	std::string opt_layout;
	short opt_fgcolor = -1; // default color
	short opt_bgcolor = -1; // default color
	auto opt_usemsec = false;

	while ((c = getopt_long(argc, argv, "c:t:mnfrvhx", lo, &i)) != -1) {
		switch (c) {
		case 'c':
			opt_layout = optarg;
			break;
		case 't':
			try {
				opt::sinterval = std::stoi(optarg);
			} catch (std::exception& e) {
				std::cerr << e.what() << ": " << optarg << std::endl;
				exit(1);
			}
			break;
		case 'F':
			opt_fgcolor = string_to_color(optarg);
			break;
		case 'B':
			opt_bgcolor = string_to_color(optarg);
			break;
		case 'm':
			opt_usemsec = true;
			break;
		case 'n':
			opt::showlnum = true;
			break;
		case 'f':
			opt::foldline = true;
			break;
		case 'N':
			opt::blinkline = false;
			break;
		case 'r':
			opt::rotatecol = true;
			break;
		case 'T':
			opt::usedelay = true;
			break;
		case 'v':
			print_version();
			exit(1);
		default:
		case 'h':
			usage(progname);
			exit(1);
		case 'x': // hidden
			print_build_options();
			exit(0);
		}
	}

	if (opt_usemsec) {
		auto x = opt::sinterval;
		opt::sinterval = x / 1000;
		opt::ninterval = (x % 1000) * 1000 * 1000;
	}

	if (!opt::sinterval && !opt::ninterval) {
		opt::sinterval = 1;
		opt::ninterval = 0;
	}

	if (opt_layout.empty()) {
		opt_layout = std::string(argc - optind, '1');
		if (opt_layout.empty())
			opt_layout = std::to_string(1);
	}
	for (auto x : opt_layout) {
		x = static_cast<char>(toupper(x));
		if (x >= '1' && x <= '9')
			opt::layout.push_back(x - '0');
		else if (x >= 'A' && x <= 'F')
			opt::layout.push_back(x - 'A' + 10);
		else
			opt::layout.push_back(-1);
	}

	init_lock();
	init_log(progname_ptr);
	init_watch(true);

	auto ret = init_screen(opt_fgcolor, opt_bgcolor);
	if (ret) {
		xlog("failed to init screen %d", ret);
		exit(1);
	}

	if (atexit(atexit_handler)) {
		perror("atexit");
		exit(1);
	}

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigint_handler;
	if (sigemptyset(&sa.sa_mask) == -1) {
		perror("sigemptyset");
		exit(1);
	}
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	try {
		Container co(std::vector<std::string>(argv + optind, argv + argc));
		if (co.thread_create())
			exit(1);
		while (!interrupted)
			co.parse_event(read_incoming());
		co.thread_join();
	} catch (const std::exception& e) {
		add_exception(e);
		exit(1);
	}

	return 0;
}
