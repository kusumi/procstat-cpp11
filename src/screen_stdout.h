#ifndef SRC_SCREEN_STDOUT_H_
#define SRC_SCREEN_STDOUT_H_

enum Key : int {
	Err = 0xDEAD,
	Up,
	Down,
	Left,
	Right,
	Resize,
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
	void erase(void) {
	}
	void resize(int, int);
	void move(int, int) {
	}
	void box(void) {
	}
	void background(void) {
	}
};
#endif // SRC_SCREEN_STDOUT_H_
