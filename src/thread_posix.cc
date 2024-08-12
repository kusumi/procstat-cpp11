#include <unistd.h>
#ifdef CONFIG_GETTIMEOFDAY
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "./thread.h"

pthread_mutex_t __mutex;

int init_lock(void) {
	return pthread_mutex_init(&__mutex, nullptr);
}

int cleanup_lock(void) {
	return pthread_mutex_destroy(&__mutex);
}

int Mutex::timedwait(time_t s, long n) {
	timespec ts;
#ifdef CONFIG_GETTIMEOFDAY
	timeval tv;
	if (gettimeofday(&tv, 0) == -1) {
#else
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
#endif
		sleep(static_cast<unsigned int>(s));
		usleep(static_cast<useconds_t>(n / 1000));
		return -1;
	}
#ifdef CONFIG_GETTIMEOFDAY
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;
#endif
	ts.tv_sec += s;
	ts.tv_nsec += n;
	if (ts.tv_nsec >= 1000000000) {
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}
	return pthread_cond_timedwait(&_c, &_m, &ts);
}
