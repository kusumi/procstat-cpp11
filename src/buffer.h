#ifndef SRC_BUFFER_H_
#define SRC_BUFFER_H_

#include <fstream>
#include <vector>
#include <string>

#include "./thread.h"

class Buffer {
	public:
	explicit Buffer(const std::string&);
	~Buffer(void) {
		_ifs.close();
	}
	const std::string& get_path(void) const {
		return _path;
	}
	int get_max_line(void);
	bool is_dead(void);
	void update(void);
	int readline(int&, std::string&, bool&);
	void clear(void);
	void block_till_ready(void);
	void signal_blocked(void);

	private:
	Mutex _mutex;
	std::vector<std::string> _chunk;
	std::ifstream _ifs;
	std::string _path;
	int _curline;
	int _maxline;
	bool _busy;
};
#endif // SRC_BUFFER_H_
