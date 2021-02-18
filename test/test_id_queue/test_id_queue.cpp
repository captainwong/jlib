#include "../../jlib/util/id_queue.h"
#include <assert.h>

template <typename Q>
void test(typename Q::value_type max_val = 100)
{
	Q q;
	typedef Q::value_type T;
	for (T i = 0; i < max_val; i++) {
		assert(q.aquire() == i);
	}
	assert(q.aquire() == max_val);
	for (T i = 0; i < max_val; i++) {
		q.release(i);
	}
	for (T i = 0; i < max_val; i++) {
		assert(q.aquire() == i);
	}
}

int main()
{
	/*jlib::IntIdQ q(0, 100);
	for (int i = 0; i < 100; i++) {
		assert(q.aquire() == i);
	}
	assert(q.aquire() == 100);
	for (int i = 0; i < 100; i++) {
		q.release(i);
	}
	for (int i = 0; i < 100; i++) {
		assert(q.aquire() == i);
	}*/

	test<jlib::IntIdQ>();
	test<jlib::IntIdQ2>();
	test<jlib::UInt32IdQ>();
	test<jlib::UInt32IdQ2>();
	test<jlib::UInt64IdQ>();
	test<jlib::UInt64IdQ2>();
}
