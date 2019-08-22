#include "../../jlib/base/singleton.h"
#include "../../jlib/base/currentthread.h"
#include <stdio.h>
#include <thread>
#include <string>



class Test
{
public:
	Test() {
		printf("tid=%lld, constructing %p\n", jlib::CurrentThread::tid(), this);
	}

	~Test() {
		printf("tid=%lld, destructing %p %s\n", jlib::CurrentThread::tid(), this, name_.c_str());
	}

	const std::string& name() const { return name_; }
	void setName(const std::string& n) { name_ = n; }

private:
	std::string name_ = {};
};

class TestNoDestroy
{
public:
	void no_destroy();

	TestNoDestroy() {
		printf("tid=%lld, constructing TestNoDestroy %p\n", jlib::CurrentThread::tid(), this);
	}

	~TestNoDestroy() {
		printf("tid=%lld, destructing TestNoDestroy %p\n", jlib::CurrentThread::tid(), this);
	}
};

void func()
{
	printf("tid=%lld, %p name=%s\n", 
		   jlib::CurrentThread::tid(), 
		   &jlib::singleton<Test>::instance(),
		   jlib::singleton<Test>::instance().name().c_str());

	jlib::singleton<Test>::instance().setName("only once, changed");
}

int main()
{
	printf("pid=%ld\n", GetCurrentProcessId());
	printf("tid=%lld\n", jlib::CurrentThread::tid());

	jlib::singleton<Test>::instance().setName("only once");
	std::thread thread(func);
	thread.join();

	printf("tid=%lld, %p name=%s\n",
			jlib::CurrentThread::tid(),
			&jlib::singleton<Test>::instance(),
			jlib::singleton<Test>::instance().name().c_str());

	jlib::singleton<TestNoDestroy>::instance();
	printf("with valgrind, you should see %zd-byte memory leak.\n", sizeof(TestNoDestroy));


	//system("pause");
}