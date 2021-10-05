export module coro.task;

import <coroutine>;
import <exception>;

export namespace coro {

struct task {
    struct promise_type {
        task get_return_object() noexcept {
            return task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return{}; }
        std::suspend_never final_suspend() noexcept { return{}; }
        void return_void() noexcept {}
        void unhandled_exception() noexcept { std::terminate(); }
    };

    explicit task(std::coroutine_handle<promise_type> h) noexcept : coro_(h) {}

    task(task&& t) noexcept : coro_(t.coro_) {
        t.coro_ = {};
    }

    ~task() {
        if (coro_ && !coro_.done()) {
            coro_.destroy();
        }
    }

    void resume() { coro_.resume(); }
    bool is_done() { return coro_.done(); }

    std::coroutine_handle<promise_type> coro_;
};

};
