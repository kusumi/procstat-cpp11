#include <sstream>
#include <iterator>
#include <algorithm>

#include "./buffer.h"
#include "./global.h"

Buffer::Buffer(const std::string& f):
	_mutex{},
	_chunk{},
	_ifs(f.c_str()),
	_path(f),
	_curline(0),
	_maxline(0),
	_busy(false) {
	update();
}

int Buffer::get_max_line(void) {
	block_till_ready();
	auto ret = _maxline;
	signal_blocked();
	return ret;
}

bool Buffer::is_dead(void) {
	block_till_ready();
	auto ret = !_ifs.is_open();
	signal_blocked();
	return ret;
}

void Buffer::update(void) {
	if (is_dead())
		return;
	block_till_ready();
	auto tmp = _ifs.tellg();
	_ifs.seekg(0);
	_maxline = static_cast<int>(std::count(
		std::istreambuf_iterator<char>(_ifs),
		std::istreambuf_iterator<char>(), '\n')) - 1;
	_ifs.seekg(tmp);
	signal_blocked();
}

// caller need to test if ready
int Buffer::readline(int& pos, std::string& s, bool& standout) {
	if (!std::getline(_ifs, s))
		return -1;

	size_t i = 0;
	while ((i = s.find("%", i)) != std::string::npos) {
		s.insert(i, "%");
		i += 2;
	}

	if (opt::blinkline) {
		if (_curline >= static_cast<int>(_chunk.size()))
			_chunk.resize(_chunk.size() * 2 + 1);
		standout = (!_chunk[_curline].empty() && _chunk[_curline] != s);
		_chunk[_curline] = s;
	} else {
		standout = false;
	}

	pos = _curline++;
	if (opt::showlnum) {
		std::ostringstream ss;
		ss << _curline << ' ' << s;
		s = ss.str();
	}
	return 0;
}

// caller need to test if ready
void Buffer::clear(void) {
	_ifs.clear();
	_ifs.seekg(0);
	_curline = 0;
}

void Buffer::block_till_ready(void) {
	_mutex.lock();
	while (_busy)
		_mutex.wait();
	_busy = true;
	_mutex.unlock();
}

void Buffer::signal_blocked(void) {
	_mutex.lock();
	_busy = false;
	_mutex.signal();
	_mutex.unlock();
}
