#pragma once

#include "config.h"
//
//#include "copyable.h"
//#include <time.h>
//#include <memory>
//#include <vector>
//#include <string>

#include "../3rdparty/date/include/date/tz.h"


namespace jlib
{

using namespace date;
using TimeZone = date::time_zone;



//namespace detail
//{
//
//struct Transition
//{
//	time_t gmtime_;
//	time_t localtime_;
//	int localTimeIdx_;
//
//	Transition(time_t gmt, time_t localt, int localIdx)
//		: gmtime_(gmt)
//		, localtime_(localt)
//		, localTimeIdx_(localIdx)
//	{}
//};
//
//struct Compare
//{
//	bool compareGmt_ = true;
//
//	Compare(bool gmt)
//		: compareGmt_(gmt)
//	{}
//
//	bool operator()(const Transition& lhs, const Transition& rhs) const {
//		if (compareGmt_) {
//			return lhs.gmtime_ < rhs.gmtime_;
//		} else {
//			return lhs.localtime_ < rhs.localtime_;
//		}
//	}
//
//	bool equal(const Transition& lhs, const Transition& rhs) const {
//		if (compareGmt_) {
//			return lhs.gmtime_ == rhs.gmtime_;
//		} else {
//			return lhs.localtime_ == rhs.localtime_;
//		}
//	}
//};
//
//struct LocalTime
//{
//	time_t gmtOffset;
//	bool isDst;
//	int abbrIdx;
//
//	LocalTime(time_t offset, bool dst, int abbr)
//		: gmtOffset(offset)
//		, isDst(dst)
//		, abbrIdx(abbr)
//	{}
//};
//
//} // namespace detail
//
//
//static constexpr int SECONDS_PER_DAY = 24 * 60 * 60;
//    
////! TimeZone for 1970~2030
//class TimeZone : public copyable
//{
//public:
//	explicit TimeZone(const char* zonename) {
//
//	}
//
//	TimeZone(int bias, const char* tzname) {
//
//	}
//
//    TimeZone() = default; // invalid timezone
//
//    bool valid() const{
//        return static_cast<bool>(data_);
//    }
//
//	//struct tm toLocalTime(time_t secondsSinceEpoch) const {
//
//	//}
//
//	//time_t fromLocalTime(const struct tm& tm_time) const {
//
//	//}
//
//	//static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
//	//static time_t fromUtcTime(const struct tm& tm_time);
//	//
//	///**
//	//* @param year [1900, 2500]
//	//* @param month [1, 12]
//	//* @param day [1, 31]
//	//* @param hour [0, 23]
//	//* @param minute [0, 59]
//	//* @param seconds [0, 59]
//	//*/
//	//static time_t fromUtcTime(int year, int month, int day,
//	//						  int hour, int minute, int seconds) {
//
//	//}
//
//	struct Data {
//		std::vector<detail::Transition> transitions;
//		std::vector<detail::LocalTime> localtimes;
//		std::vector<std::string> names;
//		std::string abbreviation;
//	};
//
//private:
//	std::shared_ptr<Data> data_ = {};
//};

} // namespace jlib
