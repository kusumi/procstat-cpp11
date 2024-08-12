#ifndef SRC_THREAD_STD_H_
#define SRC_THREAD_STD_H_

#include <thread>
#include <mutex>
#include <condition_variable>

#include <ctime>

#define EXTERN_C_BEGIN
#define EXTERN_C_END

#define get_thread_id() std::hash<std::thread::id>()(std::this_thread::get_id())

typedef void* (*pfn)(void*);
extern std::mutex __mutex;

int init_lock(void);
int cleanup_lock(void);

#if 1
static inline void global_lock(void) {
	__mutex.lock();
}

static inline void global_unlock(void) {
	__mutex.unlock();
}
#else
#define global_lock()	std::lock_guard<std::mutex> __lock(__mutex)
#define global_unlock()	do {} while (0)
#endif

class Mutex {
	public:
	explicit Mutex(void):
		_m{},
		_c{} {
	}
	int lock(void) {
		_m.lock();
		return 0;
	}
	int unlock(void) {
		_m.unlock();
		return 0;
	}
	int signal(void) {
		_c.notify_one();
		return 0;
	}
	int wait(void);
	int timedwait(time_t, long);

	private:
	std::mutex _m;
	std::condition_variable _c;
};

class Thread {
	public:
	explicit Thread(void):
		_t{} {
	}
	int create(pfn fn, void* arg) {
		_t = std::thread(fn, arg);
		return 0;
	}
	int join(void) {
		_t.join();
		return 0;
	}

	private:
	std::thread _t;
};
#endif // SRC_THREAD_STD_H_
