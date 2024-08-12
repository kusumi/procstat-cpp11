#include <filesystem>
#include <stdexcept>

#include "./global.h"
#include "./container.h"
#include "./watch.h"
#include "./log.h"

Container::Container(std::vector<std::string>&& args):
	_v{},
	_it{},
	_w(new Watcher()) {
	_build_window();
	for (size_t i = 0; i < args.size(); i++) {
		auto& s = args[i];
		try {
			auto status = std::filesystem::status(s);
			if (status.type() != std::filesystem::file_type::regular)
				continue;
		} catch (const std::filesystem::filesystem_error& e) {
			log("%s: %s", s.c_str(), e.what());
			continue;
		}
		if (i < _v.size()) {
			_v[i]->attach_buffer(s);
			auto wd = add_watch(s);
			if (wd != -1)
				_w->add(wd, _v[i]);
		}
	}
	_it = _v.begin();
	(*_it)->focus(true);
}

Container::~Container(void) {
	delete _w;
	while (!_v.empty()) {
		auto* p = _v.back();
		_v.pop_back();
		delete p;
	}
}

void Container::_goto_next_window(void) {
	std::vector<Window*>::iterator it = _it;
	(*_it)->focus(false);
	do {
		_it++;
		if (_it == _v.end())
			_it = _v.begin();
	} while ((*_it)->is_dead() && _it != it);
	(*_it)->focus(true);
}

void Container::_goto_prev_window(void) {
	std::vector<Window*>::iterator it = _it;
	(*_it)->focus(false);
	do {
		if (_it == _v.begin())
			_it = _v.end();
		_it--;
	} while ((*_it)->is_dead() && _it != it);
	(*_it)->focus(true);
}

void Container::_build_window(void) {
	if (!opt::rotatecol)
		_build_window_xy();
	else
		_build_window_yx();
}

void Container::_build_window_xy(void) {
	auto seq = 0;
	auto xx = get_terminal_cols();
	auto yy = get_terminal_lines();
	auto x = static_cast<int>(opt::layout.size());
	auto xq = xx / x;
	auto xr = xx % x;

	for (auto i = 0; i < x; i++) {
		auto xpos = xq * i;
		auto xlen = xq;
		if (i == x - 1)
			xlen += xr;
		auto y = opt::layout[i];
		if (y == -1)
			y = 1; // ignore invalid
		auto yq = yy / y;
		auto yr = yy % y;

		for (auto j = 0; j < y; j++) {
			auto ypos = yq * j;
			auto ylen = yq;
			if (j == y - 1)
				ylen += yr;
			_alloc_window(seq++, ylen, xlen, ypos, xpos);
		}
	}
}

void Container::_build_window_yx(void) {
	auto seq = 0;
	auto yy = get_terminal_lines();
	auto xx = get_terminal_cols();
	auto y = static_cast<int>(opt::layout.size());
	auto yq = yy / y;
	auto yr = yy % y;

	for (auto i = 0; i < y; i++) {
		auto ypos = yq * i;
		auto ylen = yq;
		if (i == y - 1)
			ylen += yr;
		auto x = opt::layout[i];
		if (x == -1)
			x = 1; // ignore invalid
		auto xq = xx / x;
		auto xr = xx % x;

		for (auto j = 0; j < x; j++) {
			auto xpos = xq * j;
			auto xlen = xq;
			if (j == x - 1)
				xlen += xr;
			_alloc_window(seq++, ylen, xlen, ypos, xpos);
		}
	}
}

void Container::_alloc_window(int seq, int ylen, int xlen, int ypos, int xpos) {
	try {
		auto* p = _v.at(seq);
		p->resize(ylen, xlen, ypos, xpos);
		p->signal();
	} catch (const std::out_of_range& e) {
		auto* p = new Window(ylen, xlen, ypos, xpos);
		_v.push_back(p);
	}
}

void Container::parse_event(int x) {
	auto* p = *_it;
	switch (x) {
	case Key::Err:
		break;
	case Key::Resize:
	case key_ctrl('l'):
		clear_terminal();
		_build_window();
		break;
	case 'h':
	case Key::Left:
		_goto_prev_window();
		break;
	case 'l':
	case Key::Right:
		_goto_next_window();
		break;
	case '0':
		p->goto_head();
		p->signal();
		break;
	case '$':
		p->goto_tail();
		p->signal();
		break;
	case 'k':
	case Key::Up:
		p->goto_current(-1);
		p->signal();
		break;
	case 'j':
	case Key::Down:
		p->goto_current(1);
		p->signal();
		break;
	case key_ctrl('B'):
		p->goto_current(-get_terminal_lines());
		p->signal();
		break;
	case key_ctrl('U'):
		p->goto_current(-get_terminal_lines() / 2);
		p->signal();
		break;
	case key_ctrl('F'):
		p->goto_current(get_terminal_lines());
		p->signal();
		break;
	case key_ctrl('D'):
		p->goto_current(get_terminal_lines() / 2);
		p->signal();
		break;
	default:
		p->signal();
		break;
	}
}

int Container::thread_create(void) {
	auto ret = _w->create();
	if (ret) {
		log("create watcher thread failed %d", ret);
		return ret;
	}

	for (size_t i = 0; i < _v.size(); i++) {
		ret = _v[i]->create();
		if (ret) {
			log("window=%p create thread failed %d",
				static_cast<void*>(_v[i]), ret);
			return ret;
		}
	}
	return 0;
}

void Container::thread_join(void) {
	int ret;
	for (size_t i = 0; i < _v.size(); i++)
		_v[i]->signal();
	for (size_t i = 0; i < _v.size(); i++) {
		ret = _v[i]->join();
		if (ret)
			log("window=%p join thread failed %d",
				static_cast<void*>(_v[i]), ret);
	}

	_w->signal();
	ret = _w->join();
	if (ret)
		log("join watcher thread failed %d", ret);
}
