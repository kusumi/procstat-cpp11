#ifndef SRC_LOG_H_
#define SRC_LOG_H_

#ifdef DEBUG
#include <string>

#include <syslog.h>

#include "./thread.h"

// e.g. journalctl -f _COMM=procstat-cpp11
#define log(fmt, ...)						\
do {								\
	global_lock();						\
	syslog(LOG_INFO, "<%s> " fmt, __func__, __VA_ARGS__);	\
	global_unlock();					\
} while (0)

void init_log(const char* s);
void cleanup_log(void);
#else
#define log(fmt, ...) do {} while (0)
#define init_log(s) do {} while (0)
#define cleanup_log() do {} while (0)
#endif
#endif // SRC_LOG_H_
