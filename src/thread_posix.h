#ifndef SRC_THREAD_POSIX_H_
#define SRC_THREAD_POSIX_H_

#include <ctime>

#include <pthread.h>

#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }

#define get_thread_id() static_cast<unsigned long>(pthread_self())

typedef void* (*pfn)(void*);
extern pthread_mutex_t __mutex;

int init_lock(void);
int cleanup_lock(void);

static inline void global_lock(void) {
	pthread_mutex_lock(&__mutex);
}

static inline void global_unlock(void) {
	pthread_mutex_unlock(&__mutex);
}

class Mutex {
	public:
	Mutex(void):
		_m{},
		_c{} {
		pthread_mutex_init(&_m, nullptr);
		pthread_cond_init(&_c, nullptr);
	}
	~Mutex(void) {
		pthread_cond_destroy(&_c);
		pthread_mutex_destroy(&_m);
	}
	int lock(void) {
		return pthread_mutex_lock(&_m);
	}
	int unlock(void) {
		return pthread_mutex_unlock(&_m);
	}
	int signal(void) {
		return pthread_cond_signal(&_c);
	}
	int wait(void) {
		return pthread_cond_wait(&_c, &_m);
	}
	int timedwait(time_t, long);

	private:
	pthread_mutex_t _m;
	pthread_cond_t _c;
};

class Thread {
	public:
	Thread(void):
		_t{} {
	}
	int create(pfn fn, void* arg) {
		return pthread_create(&_t, nullptr, fn, arg);
	}
	int join(void) {
		void* res;
		return pthread_join(_t, &res);
	}

	private:
	pthread_t _t;
};
#endif // SRC_THREAD_POSIX_H_
