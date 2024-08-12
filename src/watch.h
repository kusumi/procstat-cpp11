#ifndef SRC_WATCH_H_
#define SRC_WATCH_H_

#include <vector>
#include <tuple>
#include <string>

enum class WatchEvent {
	Modify = 1,
};

typedef std::vector<std::tuple<int, WatchEvent>> watch_res;

int init_watch(bool);
int cleanup_watch(void);
int add_watch(const std::string&);
int delete_watch(int);
int read_watch(watch_res&);
#endif // SRC_WATCH_H_
