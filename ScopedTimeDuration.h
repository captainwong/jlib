#pragma once

#include <chrono>

class ScopedTimeDuration 
{
public:
	ScopedTimeDuration() { _clock = std::chrono::system_clock::now(); }
	~ScopedTimeDuration(){}
private:
	std::chrono::system_clock::time_point _clock;
};

