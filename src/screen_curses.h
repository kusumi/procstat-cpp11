#ifndef SRC_SCREEN_CURSES_H_
#define SRC_SCREEN_CURSES_H_

#include <ncurses.h>

enum Key : int {
	Err = ERR,
	Up = KEY_UP,
	Down = KEY_DOWN,
	Left = KEY_LEFT,
	Right = KEY_RIGHT,
	Resize = KEY_RESIZE,
};

static constexpr inline int key_ctrl(int x) {
	return (x & 0x1F);
}

class Screen {
	public:
	explicit Screen(int, int, int, int);
	Screen(const Screen&) = delete;
	Screen(Screen&&) = delete;
	Screen& operator=(const Screen&) = delete;
	Screen& operator=(Screen&&) = delete;
	~Screen(void);
	void print(int, int, bool, const char*, ...);
	void refresh(void);
	void erase(void);
	void resize(int, int);
	void move(int, int);
	void box(void);
	void background(void);

	private:
	WINDOW* _win;
};
#endif // SRC_SCREEN_CURSES_H_
