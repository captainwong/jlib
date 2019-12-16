#pragma once

#include "config.h"
#include "../3rdparty/date/include/date/date.h"

namespace jlib {

using namespace date;
using Date = date::year_month_day;

static constexpr Date make_date_from_tm(const tm& tm) {
	return year(tm.tm_year + 1900) / month(tm.tm_mon + 1) / day(tm.tm_mday);
}

static constexpr Date make_date_from_utc_timestamp(time_t t) {
	return make_date_from_tm(*gmtime(&t));	
}

static constexpr Date make_date_from_local_timestamp(time_t t) {
	return make_date_from_tm(*localtime(&t));
}


}
