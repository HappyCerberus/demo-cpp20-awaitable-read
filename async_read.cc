module;

#include <aio.h>

export module async.read;

import <coroutine>;
import <vector>;
import <cstdint>;
import <cerrno>;
import <cstdlib>;
import <cstring>;
import <stdexcept>;
import filecontext;

export namespace async {

struct read {
	read(file_context& ctx, std::vector<uint8_t>& out) : ctx_(ctx), out_(out), caller_{}, cb_(nullptr) {
		cb_ = static_cast<aiocb*>(calloc(1,sizeof(struct aiocb)));
		if (cb_ == nullptr) throw std::runtime_error("Failed to allocate async control structure.");
	}
	~read() {
		free(cb_);
	}

	bool await_ready() { return false; }

	void await_suspend(std::coroutine_handle<> caller) {
		cb_->aio_fildes = ctx_.fd;
		cb_->aio_offset = ctx_.current_offset;
		out_.resize(ctx_.chunk_size);
		cb_->aio_buf = out_.data();
		cb_->aio_nbytes = ctx_.chunk_size;
		cb_->aio_sigevent.sigev_notify = SIGEV_THREAD;
		cb_->aio_sigevent.sigev_value.sival_ptr = this;
		cb_->aio_sigevent.sigev_notify_function = wakeup;
		caller_ = caller;	
		int err = aio_read(cb_);
        	if (err != 0) throw std::runtime_error(strerror(errno));
	}

	void await_resume() {}

private:
	static void wakeup(union sigval val) {
		read* xthis = reinterpret_cast<read*>(val.sival_ptr);
		ssize_t read = aio_return(xthis->cb_);
		if (read == -1) { throw std::runtime_error(strerror(aio_error(xthis->cb_))); }
		xthis->ctx_.current_offset += read;
		xthis->out_.resize(read);
		xthis->caller_.resume();
	}

	file_context& ctx_;
	std::vector<uint8_t>& out_;
	std::coroutine_handle<> caller_;
	struct aiocb *cb_;
};

}
