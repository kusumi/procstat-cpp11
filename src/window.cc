#include "./global.h"
#include "./window.h"
#include "./log.h"

Window::Window(int ylen, int xlen, int ypos, int xpos):
	_mutex{},
	_thread{},
	_frame(new Frame(ylen, xlen, ypos, xpos)),
	_panel(new Panel(ylen - 2, xlen - 2, ypos + 1, xpos + 1)),
	_buffer(nullptr),
	_offset(0) {
	_frame->refresh();
	_panel->refresh();
}

Window::~Window(void) {
	delete _buffer;
	delete _frame;
	delete _panel;
	log("window=%p", static_cast<void*>(this));
}

bool Window::is_dead(void) {
	return !_buffer || _buffer->is_dead();
}

void Window::resize(int ylen, int xlen, int ypos, int xpos) {
	_mutex.lock();
	_frame->resize(ylen, xlen, ypos, xpos);
	_panel->resize(ylen - 2, xlen - 2, ypos + 1, xpos + 1);
	_offset = 0;
	_mutex.unlock();
}

void Window::attach_buffer(std::string& f) {
	_mutex.lock();
	if (_buffer) {
		_mutex.unlock();
		return;
	}
	_frame->set_title(f);
	_panel->set_title(f);
	_mutex.unlock();
	_buffer = new Buffer(f);
	log("window=%p path=%s", static_cast<void*>(this),
		_buffer->get_path().c_str());
}

void Window::update_buffer(void) {
	_buffer->update();
	log("window=%p path=%s", static_cast<void*>(this),
		_buffer->get_path().c_str());
}

void Window::focus(bool t) {
	_mutex.lock();
	_frame->set_focus(t);
	_panel->set_focus(t);
	_mutex.unlock();
}

void Window::goto_head(void) {
	_mutex.lock();
	_offset = 0;
	_mutex.unlock();
}

void Window::goto_tail(void) {
	_mutex.lock();
	_offset = _buffer->get_max_line();
	_mutex.unlock();
}

void Window::goto_current(int d) {
	_mutex.lock();
	auto x = _offset + d;
	if (x < 0)
		x = 0;
	if (x > _buffer->get_max_line())
		x = _buffer->get_max_line();
	_offset = x;
	_mutex.unlock();
}

void Window::repaint(void) {
	if (is_dead())
		return;

	auto y = 0;
	int ret, pos;
	bool standout;
	std::string s;

	_mutex.lock();
	auto offset = _offset;
	auto ylen = _panel->get_ylen();
	auto xlen = _panel->get_xlen();
	_mutex.unlock();

	_buffer->block_till_ready();
	_panel->erase();
	while ((ret = _buffer->readline(pos, s, standout)) != -1) {
		_mutex.lock();
		if (y >= ylen || offset != _offset ||
			ylen != _panel->get_ylen() ||
			xlen != _panel->get_xlen()) {
			_mutex.unlock();
			break;
		}
		_mutex.unlock();
		if (pos < offset)
			continue;
		if (!opt::foldline && (static_cast<int>(s.size()) > xlen))
			s = s.substr(0, xlen);
		_panel->print(y, 0, standout, s);
		if (!opt::foldline) {
			y++;
		} else {
			auto siz = static_cast<int>(s.size());
			y += (siz / xlen);
			if (siz % xlen)
				y++;
		}
	}
	_panel->refresh();
	_buffer->clear();
	_buffer->signal_blocked();
}

EXTERN_C_BEGIN
static void* thread_handler(void* arg) {
	auto* p = reinterpret_cast<Window*>(arg);
	long t = 0;
	if (opt::usedelay) {
		auto va = reinterpret_cast<long>(p);
		if (opt::sinterval)
			t = va % 1000;
		else
			t = va % (opt::ninterval / 1000 / 1000);
	}
	log("window=%p thread=%lu delay=%ld", static_cast<void*>(p),
		get_thread_id(), t);
	if (t) {
		p->repaint();
		p->timedwait(0, t * 1000 * 1000);
	}
	while (!interrupted) {
		p->repaint();
		p->timedwait(opt::sinterval, opt::ninterval);
	}
	return nullptr;
}
EXTERN_C_END

void Window::signal(void) {
	_mutex.lock();
	_mutex.signal();
	_mutex.unlock();
}

void Window::timedwait(time_t s, long n) {
	_mutex.lock();
	_mutex.timedwait(s, n);
	_mutex.unlock();
}

int Window::create(void) {
	return _thread.create(thread_handler, this);
}
