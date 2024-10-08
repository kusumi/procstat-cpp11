#include <string>
#include <new>

#include <cstdlib>
#include <cstring>
#include <cstdarg>

#include "./screen.h"
#include "./thread.h"

namespace {
int color_attr = A_NORMAL;
int standout_attr = A_NORMAL;
}

short string_to_color(const std::string& s) {
	if (s == "black")
		return COLOR_BLACK;
	else if (s == "red")
		return COLOR_RED;
	else if (s == "green")
		return COLOR_GREEN;
	else if (s == "yellow")
		return COLOR_YELLOW;
	else if (s == "blue")
		return COLOR_BLUE;
	else if (s == "magenta")
		return COLOR_MAGENTA;
	else if (s == "cyan")
		return COLOR_CYAN;
	else if (s == "white")
		return COLOR_WHITE;
	return -1;
}

int init_screen(short fg, short bg) {
	if (!initscr())
		return -1;
	if (keypad(stdscr, TRUE) == ERR)
		return -2;
	noecho();
	cbreak();
	curs_set(0);
	wtimeout(stdscr, 500);
	clear_terminal();

	if (has_colors() == TRUE) {
		if (start_color() == ERR)
			return -4;
		if (use_default_colors() == ERR)
			return -5;
		if (init_pair(1, fg, bg) == ERR)
			return -6;
		color_attr = COLOR_PAIR(1);
	}

	const auto* env = getenv("TERM");
	if (env && !strcmp(env, "screen"))
		standout_attr = A_REVERSE;
	else
		standout_attr = A_STANDOUT;
	return 0;
}

int cleanup_screen(void) {
	curs_set(1);
	if (endwin() == ERR)
		return -1;
	return 0;
}

int read_incoming(void) {
	return wgetch(stdscr);
}

void clear_terminal(void) {
	global_lock();
	wclear(stdscr);
	wrefresh(stdscr);
	global_unlock();
}

int get_terminal_lines(void) {
	global_lock();
	auto y = static_cast<int>(LINES);
	global_unlock();
	return y;
}

int get_terminal_cols(void) {
	global_lock();
	auto x = static_cast<int>(COLS);
	global_unlock();
	return x;
}

void flash_terminal(void) {
	flash();
}

Screen::Screen(int ylen, int xlen, int ypos, int xpos):
	_win(nullptr) {
	global_lock();
	_win = newwin(ylen, xlen, ypos, xpos);
	if (_win) {
		scrollok(_win, FALSE);
		idlok(_win, FALSE);
		keypad(_win, TRUE);
	}
	global_unlock();
	if (!_win)
		throw std::bad_alloc();
}

Screen::~Screen(void) {
	global_lock();
	delwin(_win);
	global_unlock();
}

void Screen::print(int y, int x, bool standout, const char* fmt, ...) {
	auto attr = standout ? standout_attr : A_NORMAL;
	global_lock();
	wattron(_win, attr);
	va_list ap;
	va_start(ap, fmt);
	mvwprintw(_win, y, x, fmt, ap);
	va_end(ap);
	wattroff(_win, attr);
	global_unlock();
}

void Screen::refresh(void) {
	global_lock();
	wrefresh(_win);
	global_unlock();
}

void Screen::erase(void) {
	global_lock();
	werase(_win);
	global_unlock();
}

void Screen::resize(int ylen, int xlen) {
	global_lock();
	wresize(_win, ylen, xlen);
	global_unlock();
}

void Screen::move(int ypos, int xpos) {
	global_lock();
	mvwin(_win, ypos, xpos);
	global_unlock();
}

void Screen::box(void) {
	global_lock();
	wborder(_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
		ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	global_unlock();
}

void Screen::background(void) {
	global_lock();
	if (color_attr != A_NORMAL)
		wbkgd(_win, color_attr | ' ');
	global_unlock();
}
