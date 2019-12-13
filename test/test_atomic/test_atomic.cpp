#include <atomic>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <assert.h>

void test()
{	
	using namespace std;
	using namespace std::chrono;

	uint32_t count = 0;
	while (true) {
		atomic_int x(0), y(0);
		int r1 = 0, r2 = 0;

		thread t1([&x, &y, &r1, &r2]() {
			// Thread 1:
			this_thread::sleep_for(0ms); 
			r1 = y.load(memory_order_relaxed); // A
			x.store(r1, memory_order_relaxed); // B
		});
		
		thread t2([&x, &y, &r1, &r2]() {
			// Thread 2:
			this_thread::sleep_for(0ms);
			r2 = x.load(memory_order_relaxed); // C 
			y.store(42, memory_order_relaxed); // D
		});

		t1.join();
		t2.join();
		count++;

		if (r1 == 42 && r2 == 42) {
			break;
		}
	}
	
	printf("after %d times trying, r1,r2 are finally 42!\n", count);
}

int main()
{
	test();


	/*{
		std::atomic_int32_t a0;
		assert(a0.load() == 0);
		assert(a0.fetch_add(1) == 0);
		assert(a0.load() == 1);
	}*/
}
