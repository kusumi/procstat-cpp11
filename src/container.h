#ifndef SRC_CONTAINER_H_
#define SRC_CONTAINER_H_

#include <vector>
#include <string>

#include "./window.h"
#include "./watcher.h"

class Container {
	public:
	explicit Container(std::vector<std::string>&&);
	Container(const Container&) = delete;
	Container(Container&&) = delete;
	Container& operator=(const Container&) = delete;
	Container& operator=(Container&&) = delete;
	~Container(void);
	void parse_event(int);
	int thread_create(void);
	void thread_join(void);

	private:
	void _goto_next_window(void);
	void _goto_prev_window(void);
	void _build_window(void);
	void _build_window_xy(void);
	void _build_window_yx(void);
	void _alloc_window(int, int, int, int, int);
	std::vector<Window*> _v;
	std::vector<Window*>::iterator _it;
	Watcher *_w;
};
#endif // SRC_CONTAINER_H_
