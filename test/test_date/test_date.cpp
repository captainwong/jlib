#include "../../jlib/base/date.h"
#include <stdio.h>
#include <assert.h>

using namespace jlib;

const int kMonthsOfYear = 12;

int isLeapYear(int year)
{
	if (year % 400 == 0)
		return 1;
	else if (year % 100 == 0)
		return 0;
	else if (year % 4 == 0)
		return 1;
	else
		return 0;
}

int daysOfMonth(int year, int month)
{
	static int days[2][kMonthsOfYear + 1] =
	{
	  { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	  { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	};
	return days[isLeapYear(year)][month];
}

void passByConstReference(const Date& x)
{
	printf("passByConstReference: %s\n", format("%F", x).c_str());
}

void passByValue(Date x)
{
	printf("passByValue: %s\n", format("%F", x).c_str());
}

int main()
{
	Date someDay(2008_y, sep, 10_d);
	printf("someDay: %s\n", format("%F", someDay).c_str());
	passByValue(someDay);
	passByConstReference(someDay);

	time_t now = time(nullptr);
	Date todayUtc = make_date_from_utc_timestamp(now);
	printf("todayUtc: %s\n", format("%F %A", todayUtc).c_str());
	Date todayLocal = make_date_from_local_timestamp(now);
	printf("todayLocal: %s\n", format("%F %A", todayLocal).c_str());

	Date epoch(sys_days(days(0)));
	printf("epoch: %s week:%d days_since_epoch:%d\n", 
		   format("%F", epoch).c_str(), 
		   year_month_weekday(epoch).weekday().c_encoding(),
		   sys_days(epoch).time_since_epoch().count());	

	int daysSinceEpoch = 0;
	int weekDay = 4; 

	for (int y = 1970; y < 2500; ++y) {
		assert((sys_days(year(y)/3/1) - sys_days(year(y)/2/29)).count() == isLeapYear(y));
		for (int m = 1; m <= kMonthsOfYear; ++m) {
			for (int d = 1; d <= daysOfMonth(y, m); ++d) {
				Date date = year(y)/m/d;
				// printf("%s %d\n", d.toString().c_str(), d.weekd());
				assert(y == (int)(date.year()));
				assert(m == (int)(unsigned)(date.month()));
				assert(d == (int)(unsigned)date.day());
				assert(weekDay == year_month_weekday(date).weekday().c_encoding());
				assert(daysSinceEpoch == sys_days(date).time_since_epoch().count());

				Date date2 = sys_days(days(daysSinceEpoch)); 
				assert(date == date2);
				assert(y == (int)(date2.year()));
				assert(m == (int)(unsigned)date2.month());
				assert(d == (int)(unsigned)date2.day());
				assert(weekDay == year_month_weekday(date2).weekday().c_encoding());
				assert(daysSinceEpoch == sys_days(date2).time_since_epoch().count());

				++daysSinceEpoch;
				weekDay = (weekDay + 1) % 7;
			}
		}
	}

	printf("All passed.\n");
}
