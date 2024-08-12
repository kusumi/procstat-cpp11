#include <vector>
#include <string>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>

#include <unistd.h>
#include <sys/ioctl.h>

#include "./screen.h"
#include "./thread.h"

namespace {
int LINES = -1;
int COLS = -1;

void update_terminal_size(void) {
	global_lock();
	winsize ws;
	if (ioctl(0, TIOCGWINSZ, &ws) == -1) {
		perror("ioctl");
		exit(1);
	}
	LINES = static_cast<int>(ws.ws_row);
	COLS = static_cast<int>(ws.ws_col);
	global_unlock();
}
} // namespace

short string_to_color([[maybe_unused]] const std::string& s) {
	return -1;
}

int init_screen([[maybe_unused]] short fg, [[maybe_unused]] short bg) {
	update_terminal_size();
	return 0;
}

int cleanup_screen(void) {
	return 0;
}

int read_incoming(void) {
	sleep(1);
	return Key::Err;
}

void clear_terminal(void) {
}

int get_terminal_lines(void) {
	assert(LINES != -1);
	return LINES;
}

int get_terminal_cols(void) {
	assert(COLS != -1);
	return COLS;
}

void flash_terminal(void) {
}

Screen::Screen([[maybe_unused]] int ylen, [[maybe_unused]] int xlen,
	[[maybe_unused]] int ypos, [[maybe_unused]] int xpos) {
	global_lock();
	fprintf(stdout, "Allocate %p\n", reinterpret_cast<void*>(this));
	global_unlock();
}

Screen::~Screen(void) {
	global_lock();
	fprintf(stdout, "Delete %p\n", reinterpret_cast<void*>(this));
	global_unlock();
}

void Screen::print([[maybe_unused]] int y, [[maybe_unused]] int x,
	[[maybe_unused]] bool standout, const char* fmt, ...) {
	global_lock();
	fprintf(stdout, "%p \"", reinterpret_cast<void*>(this));
	va_list ap;
	va_start(ap, fmt);
	fprintf(stdout, fmt, ap);
	va_end(ap);
	fprintf(stdout, "\"\n");
	global_unlock();
}

void Screen::refresh(void) {
	global_lock();
	fflush(stdout);
	global_unlock();
}

void Screen::resize([[maybe_unused]] int ylen, [[maybe_unused]] int xlen) {
	update_terminal_size();
}
