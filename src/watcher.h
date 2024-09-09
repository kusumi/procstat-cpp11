#ifndef SRC_WATCHER_H_
#define SRC_WATCHER_H_

#include <map>

#include "./thread.h"
#include "./watch.h"
#include "./window.h"

typedef std::map<int, Window*> map_t;

class Watcher {
	public:
	Watcher(void):
		_mutex{},
		_thread{},
		_map{} {
	}
	~Watcher(void);
	void add(int wd, Window* w) {
		_map.insert(map_t::value_type(wd, w));
	}
	void parse_event(int, WatchEvent);
	void signal(void) {
		_mutex.signal();
	}
	void timedwait(time_t s, long n) {
		_mutex.timedwait(s, n);
	}
	int create(void);
	int join(void) {
		return _thread.join();
	}

	private:
	Mutex _mutex;
	Thread _thread;
	map_t _map;
};
#endif // SRC_WATCHER_H_
