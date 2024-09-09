#ifndef SRC_WINDOW_H_
#define SRC_WINDOW_H_

#include <string>
#include <memory>

#include "./buffer.h"
#include "./panel.h"
#include "./thread.h"

class Window {
	public:
	explicit Window(int, int, int, int);
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;
	~Window(void);
	bool is_dead() {
		return !_buffer || _buffer->is_dead();
	}
	void resize(int, int, int, int);
	void attach_buffer(const std::string&);
	void update_buffer(void);
	void focus(bool);
	void goto_head(void);
	void goto_tail(void);
	void goto_current(int);
	void repaint(void);
	void signal(void);
	void timedwait(time_t, long);
	int create(void);
	int join(void) {
		return _thread.join();
	}

	private:
	Mutex _mutex;
	Thread _thread;
	std::unique_ptr<Panel> _frame;
	std::unique_ptr<Panel> _panel;
	std::unique_ptr<Buffer> _buffer;
	int _offset;
};
#endif // SRC_WINDOW_H_
