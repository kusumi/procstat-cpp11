#include <vector>

#include <cerrno>

#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>

#include "./watch.h"

namespace {
int fd = -1;
std::vector<char> buf;

void expand_buffer(int n) {
	buf.resize(buf.size() + n * (sizeof(inotify_event) + 256));
}
}

int init_watch(bool nonblock) {
	fd = inotify_init();
	if (fd == -1)
		return -1;
	if (nonblock) {
		auto ret = fcntl(fd, F_GETFL, 0);
		if (ret == -1 || fcntl(fd, F_SETFL, ret | O_NONBLOCK) == -1) {
			close(fd);
			return -1;
		}
	}
	return 0;
}

int cleanup_watch(void) {
	return close(fd);
}

int add_watch(const std::string& path) {
	auto ret = inotify_add_watch(fd, path.c_str(), IN_MODIFY);
	if (ret != -1)
		expand_buffer(1);
	return ret;
}

int delete_watch(int wd) {
	auto ret = inotify_rm_watch(fd, wd);
	if (ret != -1)
		expand_buffer(-1);
	return ret;
}

int read_watch(watch_res& r) {
	auto* p = &buf[0];
	auto ret = read(fd, p, buf.size());
	if (ret == -1) {
		if (errno == EAGAIN)
			return 0;
		else
			return -1;
	}
	auto i = 0;
	while (i < ret) {
		auto* e = reinterpret_cast<inotify_event*>(p + i);
		if (e->mask & IN_MODIFY)
			r.emplace_back(e->wd, WatchEvent::Modify);
		i += sizeof(inotify_event) + e->len;
	}
	return static_cast<int>(ret);
}
