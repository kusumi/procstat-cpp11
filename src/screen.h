#ifndef SRC_SCREEN_H_
#define SRC_SCREEN_H_

#include <string>

#ifdef CONFIG_STDOUT
#include "./screen_stdout.h"
#else
#include "./screen_curses.h"
#endif

short string_to_color(const std::string&);
int init_screen(short, short);
int cleanup_screen(void);

int read_incoming(void);
void clear_terminal(void);
int get_terminal_lines(void);
int get_terminal_cols(void);
void flash_terminal(void);
#endif // SRC_SCREEN_H_
