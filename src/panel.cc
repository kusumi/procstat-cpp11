#include "./panel.h"

// Panel
Panel::Panel(int ylen, int xlen, int ypos, int xpos):
	_scr(new Screen(ylen, xlen, ypos, xpos)),
	_ylen(ylen),
	_xlen(xlen),
	_ypos(ypos),
	_xpos(xpos) {
	_scr->background();
}

void Panel::resize(int ylen, int xlen, int ypos, int xpos) {
	_resize(ylen, xlen, ypos, xpos);
	refresh();
}

void Panel::_resize(int ylen, int xlen, int ypos, int xpos) {
	_ylen = ylen;
	_xlen = xlen;
	_ypos = ypos;
	_xpos = xpos;
	_scr->resize(_ylen, _xlen);
	_scr->move(_ypos, _xpos);
}

void Panel::print(int y, int x, bool standout, std::string& s) {
	_scr->print(y, x, standout, s.c_str());
}

// Frame
Frame::Frame(int ylen, int xlen, int ypos, int xpos):
	Panel(ylen, xlen, ypos, xpos),
	_title{},
	_focus(false) {
	_scr->box();
}

void Frame::set_title(std::string& s) {
	_title = s;
	_print_title();
}

void Frame::set_focus(bool t) {
	_focus = t;
	_print_title();
}

void Frame::resize(int ylen, int xlen, int ypos, int xpos) {
	_resize(ylen, xlen, ypos, xpos);
	_scr->box();
	_print_title();
}

void Frame::_print_title(void) {
	print(0, 1, _focus, _title);
	refresh();
}
