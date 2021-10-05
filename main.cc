import async.op;
import coro.task;
import filecontext;
import <coroutine>;
import <vector>;
import <cstdint>;

coro::task my_coro() {
	auto fc = async::file_context::open_ro("main.cc");
	std::vector<uint8_t> data;
	co_await async::read(fc, data);
//	for (auto x : data) {
//		std::cout << static_cast<char>(x);
//	}
//	std::cout << "\n";
	co_return;
}

int main() {
	auto t = my_coro();
}
