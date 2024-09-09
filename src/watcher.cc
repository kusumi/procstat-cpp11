#include "./global.h"
#include "./log.h"
#include "./watcher.h"

Watcher::~Watcher(void) {
	for (const auto& e : _map)
		delete_watch(e.first);
}

void Watcher::parse_event(int wd, WatchEvent event) {
	if (!_map.contains(wd))
		return;
	switch (event) {
	case WatchEvent::Modify:
		_map[wd]->update_buffer();
		break;
	default:
		break;
	}
}

namespace {
EXTERN_C_BEGIN
void* watch_handler_impl(Watcher* p) {
	xlog("thread=%lu", get_thread_id());
	while (!interrupted) {
		watch_res r;
		if (read_watch(r) > 0) {
			for (const auto& e : r) {
				auto [wd, event] = e;
				p->parse_event(wd, event);
			}
			flash_terminal();
		}
		p->timedwait(1, 0);
	}
	return nullptr;
}

void* watch_handler(void* arg) {
	try {
		return watch_handler_impl(reinterpret_cast<Watcher*>(arg));
	} catch (const std::exception& e) {
		add_exception(e);
		return nullptr;
	}
}
EXTERN_C_END
} // namespace

int Watcher::create(void) {
	return _thread.create(watch_handler, this);
}
