#ifndef SRC_GLOBAL_H_
#define SRC_GLOBAL_H_

#include <vector>

#include <csignal>
#include <ctime>

extern volatile sig_atomic_t interrupted;

// readonly after getopt
namespace opt {
	extern std::vector<int> layout;
	extern time_t sinterval;
	extern long ninterval;
	extern bool showlnum;
	extern bool foldline;
	extern bool blinkline;
	extern bool rotatecol;
	extern bool usedelay;
}
#endif // SRC_GLOBAL_H_
