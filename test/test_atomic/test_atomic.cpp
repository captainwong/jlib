#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <assert.h>

// I can't produce even once...
// see https://stackoverflow.com/questions/52199869/how-to-test-the-behavior-of-stdmemory-order-relaxed
void test()
{	
	using namespace std;
	using namespace std::chrono;

	uintmax_t count = 0;
	while (true) {
		atomic_int x(0), y(0);
		int r1 = 0, r2 = 0;

		thread t1([&x, &y, &r1, &r2]() {
			// Thread 1:
			r1 = y.load(memory_order_relaxed); // A
			x.store(r1, memory_order_relaxed); // B
		});
		
		thread t2([&x, &y, &r1, &r2]() {
			// Thread 2:
			r2 = x.load(memory_order_relaxed); // C 
			y.store(42, memory_order_relaxed); // D
		});

		t1.join();
		t2.join();
		count++;

		printf("%lld\r", count);
		if (r1 == 42 && r2 == 42) {
			printf("\n");
			break;
		}
	}
	
	printf("after %lld times trying, r1,r2 are finally 42!\n", count);
}

// Typical use for relaxed memory ordering is incrementing counters, 
// such as the reference counters of std::shared_ptr, 
// since this only requires atomicity, but not ordering or synchronization 
// (note that decrementing the shared_ptr counters requires acquire-release synchronization with the destructor).
void test2()
{
	std::atomic<int> cnt = { 0 };

	auto f = [&cnt]() {
		for (int n = 0; n < 1000; ++n) {
			cnt.fetch_add(1, std::memory_order_relaxed);
		}
	};

	std::vector<std::thread> v;
	for (int n = 0; n < 1000; ++n) {
		v.emplace_back(f);
	}
	for (auto& t : v) {
		t.join();
	}
	std::cout << "Final counter value is " << cnt << '\n';
	assert(cnt.load() == 1000000);
}

// Mutual exclusion locks, such as std::mutex or atomic spinlock, 
// are an example of release-acquire synchronization: 
// when the lock is released by thread A and acquired by thread B, 
// everything that took place in the critical section (before the release)
// in the context of thread A has to be visible to thread B (after the acquire)
// which is executing the same critical section.
void test_release_acquire_synchronization()
{
	std::atomic<std::string*> ptr{};
	int data{};

	auto producer = [&ptr, &data]() {
		std::string* p = new std::string("Hello");
		data = 42;
		ptr.store(p, std::memory_order_release);
	};

	auto consumer = [&ptr, &data]() {
		std::string* p2;
		while (!(p2 = ptr.load(std::memory_order_acquire)))
			;
		assert(*p2 == "Hello"); // never fires
		assert(data == 42); // never fires
	};
	
	std::thread t1(producer);
	std::thread t2(consumer);
	t1.join(); t2.join();
}

void test3() {
	for (int i = 0; i < 10000; i++) { test_release_acquire_synchronization(); }
}

namespace test_compare_exchange_weak
{
	template<typename T>
	struct node
	{
		T data;
		node* next;
		node(const T& data) : data(data), next(nullptr) {}
	};

	template<typename T>
	class stack
	{
		std::atomic<node<T>*> head = {};
	public:
		void push(const T& data)
		{
			node<T>* new_node = new node<T>(data);

			// put the current value of head into new_node->next
			new_node->next = head.load(std::memory_order_relaxed);

			// now make new_node the new head, but if the head
			// is no longer what's stored in new_node->next
			// (some other thread must have inserted a node just now)
			// then put that new head into new_node->next and try again
			while (!head.compare_exchange_weak(new_node->next, new_node,
											   std::memory_order_release,
											   std::memory_order_relaxed))
				; // the body of the loop is empty

			// Note: the above use is not thread-safe in at least 
			// GCC prior to 4.8.3 (bug 60272), clang prior to 2014-05-05 (bug 18899)
			// MSVC prior to 2014-03-17 (bug 819819). The following is a workaround:
			//      node<T>* old_head = head.load(std::memory_order_relaxed);
			//      do {
			//          new_node->next = old_head;
			//       } while(!head.compare_exchange_weak(old_head, new_node,
			//                                           std::memory_order_release,
			//                                           std::memory_order_relaxed));
		}

		node<T>* Head() const { return head.load(std::memory_order_relaxed); }
	};

	void basic_test()
	{
		stack<int> s;
		s.push(1);
		s.push(2);
		s.push(3);
		auto head = s.Head();
		assert(head->data == 3); 
		head = head->next;
		assert(head->data == 2);
		head = head->next;
		assert(head->data == 1);
		head = head->next;
		assert(head == nullptr);		
	}

	void test() {
		using namespace std::chrono;
		stack<int> s;
		std::vector<std::thread> threads;
		for (int i = 0; i < 1000; i++) {			
			threads.emplace_back(std::thread([&s, i]() {
				std::this_thread::sleep_for(1ms);
				s.push(i);
			}));
		}
		for (auto& t : threads) { t.join(); }

		int i, count = 1;
		auto p = s.Head(); i = p->data;
		while (p = p->next) {
			if (i < p->data) {
				printf("reversed i=%d, p=%d\n", i, p->data);
				i = p->data;
			} else if (i == p->data) {
				assert(0);
			}
			count++;
		}
		assert(count == 1000);
	}
}

void test4() {
	test_compare_exchange_weak::basic_test();
	test_compare_exchange_weak::test();
}

void test_compare_exchange_strong() {
	std::atomic<int>  ai;

	int  tst_val = 4;
	int  new_val = 5;
	bool exchanged = false;

	auto valsout = [&]() {
		std::cout << "ai= " << ai
			<< "  tst_val= " << tst_val
			<< "  new_val= " << new_val
			<< "  exchanged= " << std::boolalpha << exchanged
			<< "\n";
	};

	ai = 3;
	valsout();
	assert(ai.load() == 3);
	assert(tst_val == 4);
	assert(new_val == 5);
	assert(exchanged == false);

	// tst_val != ai   ==>  tst_val is modified
	exchanged = ai.compare_exchange_strong(tst_val, new_val);
	valsout();
	assert(ai.load() == 3);
	assert(tst_val == 3);
	assert(new_val == 5);
	assert(exchanged == false);

	// tst_val == ai   ==>  ai is modified
	exchanged = ai.compare_exchange_strong(tst_val, new_val);
	valsout();
	assert(ai.load() == 5);
	assert(tst_val == 3);
	assert(new_val == 5);
	assert(exchanged == true);
}

void test5() {
	for (int i = 0; i < 1000; i++) {
		test_compare_exchange_strong();
	}
}

void test_transitive_release_acquire_ordering()
{
	std::vector<int> data;
	std::atomic<int> flag = { 0 };

	auto thread_1 = [&]() {
		data.push_back(42);
		flag.store(1, std::memory_order_release);
	};

	auto thread_2 = [&]() {
		int expected = 1;
		while (!flag.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
			expected = 1;
		}
	};

	auto thread_3 = [&]() {
		while (flag.load(std::memory_order_acquire) < 2)
			;
		assert(data.at(0) == 42); // will never fire
	};

	std::thread a(thread_1);
	std::thread b(thread_2);
	std::thread c(thread_3);
	a.join(); b.join(); c.join();
}

void test6() {
	for (int i = 0; i < 10000; i++) {
		test_transitive_release_acquire_ordering();
	}
}

bool test_memory_order_consume()
{
	std::atomic<std::string*> ptr{};
	int data{};

	auto producer = [&]() {
		std::string* p = new std::string("Hello");
		data = 42;
		ptr.store(p, std::memory_order_release);
	};

	bool data_is_42 = false;

	auto consumer = [&]() {
		std::string* p2;
		while (!(p2 = ptr.load(std::memory_order_consume)))
			;
		assert(*p2 == "Hello"); // never fires: *p2 carries dependency from ptr
		//assert(data == 42); // may or may not fire: data does not carry dependency from ptr
		data_is_42 = data == 42;
	};

	std::thread t1(producer);
	std::thread t2(consumer);
	t1.join(); t2.join();

	return data_is_42;
}

void test7()
{
	int ok = 0;
	for (int i = 0; i < 10000; i++) {
		if (test_memory_order_consume()) {
			ok++;
		}
	}
	printf("test_memory_order_consume data is 42: %d/10000\n", ok);
}

int test_memory_order_seq_cst() {
	std::atomic<bool> x = { false };
	std::atomic<bool> y = { false };
	std::atomic<int> z = { 0 };

	auto write_x = [&x, &y, &z]() {
		x.store(true, std::memory_order_seq_cst);
	};

	auto write_y = [&x, &y, &z]() {
		y.store(true, std::memory_order_seq_cst);
	};

	auto read_x_then_y = [&x, &y, &z]() {
		while (!x.load(std::memory_order_seq_cst))
			;
		if (y.load(std::memory_order_seq_cst)) {
			++z;
		}
	};

	auto read_y_then_x = [&x, &y, &z]() {
		while (!y.load(std::memory_order_seq_cst))
			;
		if (x.load(std::memory_order_seq_cst)) {
			++z;
		}
	};

	std::thread a(write_x);
	std::thread b(write_y);
	std::thread c(read_x_then_y);
	std::thread d(read_y_then_x);
	a.join(); b.join(); c.join(); d.join();
	assert(z.load() != 0);  // will never happen
	return z.load();
}

void test8() {
	for (int i = 0; i < 50; i++) {
		printf("test_memory_order_seq_cst z=%d\n", test_memory_order_seq_cst());
	}
}

void muduo_test() {
	{
		std::atomic_int64_t a0{ 0 };
		assert(a0.load() == 0);
		assert(a0.fetch_add(1) == 0);
		assert(a0.load() == 1);
		assert(a0.fetch_add(2) + 2 == 3);
		assert(a0.load() == 3);
		assert(++a0 == 4);
		assert(a0.load() == 4);
		a0++;
		assert(a0.load() == 5);
		assert(a0.fetch_add(-3) - 3 == 2);
		int64_t expected = 100;
		a0.compare_exchange_strong(expected, 100);
		assert(expected == 2);
		a0.compare_exchange_strong(expected, 100);
		assert(a0.load() == 100);
	}

	{
		std::atomic_int32_t a0{ 0 };
		assert(a0.load() == 0);
		assert(a0.fetch_add(1) == 0);
		assert(a0.load() == 1);
		assert(a0.fetch_add(2) + 2 == 3);
		assert(a0.load() == 3);
		assert(++a0 == 4);
		assert(a0.load() == 4);
		a0++;
		assert(a0.load() == 5);
		assert(a0.fetch_add(-3) - 3 == 2);
		int expected = 100;
		a0.compare_exchange_strong(expected, 100);
		assert(expected == 2);
		a0.compare_exchange_strong(expected, 100);
		assert(a0.load() == 100);
	}
}

int main()
{
	//test(); // most likely infinite
	//test2();
	//test3();
	//test4();
	//test5();
	//test6();
	//test7();
	//test8();

	muduo_test();
	
}
