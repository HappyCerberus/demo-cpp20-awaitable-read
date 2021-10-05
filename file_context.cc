module;

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

export module filecontext;

import <optional>;
import <cstdint>;
import <string>;
import <cerrno>;

export namespace async {

struct file_context {
	off_t current_offset = 0;
	uint64_t chunk_size = 512*1024;
	int fd = -1;
	std::optional<std::string> err;

	operator bool() const { return is_valid(); }
	void advance(off_t offset) { current_offset += offset; }
	bool is_valid() const { return fd != -1; }

	~file_context() {
		if (fd != -1) close(fd); 
	}

	file_context(const file_context&) = delete;
	file_context& operator = (const file_context&) = delete;

	file_context(file_context&& other) : 
		current_offset(other.current_offset),
		chunk_size(other.chunk_size),
		fd(other.fd),
		err(other.err) {
			other.fd = -1;
	}

	file_context& operator = (file_context&& other) {
		if (this == &other) return *this;
		current_offset = other.current_offset;
		chunk_size = other.chunk_size;
		fd = other.fd;
		err = other.err;
		other.fd = -1;
		return *this;
	}

	static file_context open_ro(const std::string& fn) {
		int fd = open(fn.c_str(), O_RDONLY);
		if (fd == -1) {
			return {0,0,-1,std::string(strerror(errno))};				
		}
		return {0,512*1024,fd,{}};
	}

	static file_context open_wo(const std::string& fn) {
		int fd = open(fn.c_str(), O_WRONLY | O_CREAT);
		if (fd == -1) {
			return {0,0,-1,std::string(strerror(errno))};				
		}
		return {0,512*1024,fd,{}};
	}

	private:
	file_context(off_t offset, uint64_t chunk_size, int fd, std::optional<std::string> err) 
	  : current_offset(offset), chunk_size(chunk_size), fd(fd), err(err) {}
};

}
