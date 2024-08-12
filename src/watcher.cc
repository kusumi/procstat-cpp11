#include "./global.h"
#include "./watcher.h"
#include "./log.h"

Watcher::~Watcher(void) {
	for (const auto& e : _map)
		delete_watch(e.first);
}

void Watcher::parse_event(int wd, WatchEvent event) {
	if (_map.find(wd) == _map.end())
		return;
	switch (event) {
	case WatchEvent::Modify:
		_map[wd]->update_buffer();
		break;
	default:
		break;
	}
}

EXTERN_C_BEGIN
static void* watch_thread_handler(void* arg) {
	auto* p = reinterpret_cast<Watcher*>(arg);
	log("thread=%lu", get_thread_id());
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
EXTERN_C_END

int Watcher::create(void) {
	return _thread.create(watch_thread_handler, this);
}
