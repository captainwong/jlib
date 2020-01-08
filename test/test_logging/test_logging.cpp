#include "../../jlib/base/logging.h"
#include "../../jlib/base/threadpool.h"
#include "../../jlib/base/timezone.h"

using namespace jlib;

int main()
{
	Logger::setLogLevel(Logger::LOGLEVEL_TRACE);

	LOG_TRACE << "trace";
	LOG_DEBUG << "debug";
	LOG_INFO << "Hello";
	LOG_WARN << "World";
	LOG_ERROR << "Error";
	LOG_INFO << sizeof(Logger);
	LOG_INFO << sizeof(LogStream);
	LOG_INFO << sizeof(Format);
	LOG_INFO << sizeof(LogStream::Buffer);
}