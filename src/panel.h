#ifndef SRC_PANEL_H_
#define SRC_PANEL_H_

#include <string>

#include "./screen.h"

class Panel {
	public:
	explicit Panel(int, int, int, int);
	Panel(const Panel&) = delete;
	Panel(Panel&&) = delete;
	Panel& operator=(const Panel&) = delete;
	Panel& operator=(Panel&&) = delete;
	virtual ~Panel(void) = default;
	int get_ylen(void) const {
		return _ylen;
	}
	int get_xlen(void) const {
		return _xlen;
	}
	int get_ypos(void) const {
		return _ypos;
	}
	int get_xpos(void) const {
		return _xpos;
	}
	virtual void set_title(const std::string&) {
	}
	virtual void set_focus(bool) {
	}
	virtual void resize(int, int, int, int);
	void refresh(void) {
		_scr.refresh();
	}
	void erase(void) {
		_scr.erase();
	}
	void print(int y, int x, bool standout, const std::string& s) {
		_scr.print(y, x, standout, s.c_str());
	}

	protected:
	void _resize(int, int, int, int);
	Screen _scr;
	int _ylen, _xlen, _ypos, _xpos;
};

class Frame: public Panel {
	public:
	explicit Frame(int, int, int, int);
	void set_title(const std::string&) override;
	void set_focus(bool) override;
	void resize(int, int, int, int) override;

	private:
	void _print_title(void);
	std::string _title;
	bool _focus;
};
#endif // SRC_PANEL_H_
