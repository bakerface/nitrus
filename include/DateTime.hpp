/*
 * Copyright (c) 2012 Christopher M. Baker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef DATETIME_HPP_
#define DATETIME_HPP_

#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <time.h>

#include "StackTrace.hpp"
#include "TimeSpan.hpp"

#ifdef _WIN32
# include <windows.h>

struct timezone {
	int tz_minuteswest; /* of Greenwich */
	int tz_dsttime;     /* type of daylight savings time correction */
};

/**
 * Gets the current system time with microsecond resolution.
 * @param tv A structure representing the number of seconds and microseconds since the Unix epoch.
 * @param tz A structure representing the system's timezone offset including daylight savings time.
 * @return This function will always return zero.
 */
int gettimeofday(struct timeval* tv, struct timezone* tz) {
	static bool isTimezoneSet = false;

	if (tv) {
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);

		LARGE_INTEGER large;
		large.LowPart  = ft.dwLowDateTime;
		large.HighPart = ft.dwHighDateTime;
		uint64_t microseconds = large.QuadPart;
		microseconds -= 116444736000000000LL;
		microseconds /= 10;

		tv->tv_sec = (long)(microseconds / 1000000);
		tv->tv_usec = (long)(microseconds % 1000000);
	}

	if (tz) {

		if (isTimezoneSet == false) {
		  _tzset();
		  isTimezoneSet = true;
		}

		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}

#else
# include <sys/time.h>
#endif

namespace nitrus {

/**
 * A class that provides functionality for date and time operations.
 */
class DateTime {
private:
	TimeSpan _timeSpan;

private:

	/**
	 * Creates a new date and time from a given time span.
	 *
	 * @param timeSpan The amount of time since the January 1, 0000.
	 */
	DateTime(TimeSpan timeSpan) : _timeSpan(timeSpan) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Calculates the number of days since January 1 before or after a month.
	 *
	 * @param month The one-based month.
	 * @param before True if this function should return the number of days before the start of the month.
	 * @return The number of days before the start of the month if before is true, or the number of days after the month if false.
	 */
	static uint16_t YearDaysBeforeOrAfterMonth(uint8_t month, bool before) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static uint16_t days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
		return days[before ? month - 1 : month];
	}

	/**
	 * Determines whether or not a particular year is a leap year.
	 *
	 * @param year The year.
	 * @return True if the year is a leap year, false otherwise.
	 */
	static bool LeapYear(uint16_t year) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (year % 4 == 0) {
			if (year % 100 == 0) {
				if (year % 400 == 0) {
					return true;
				}
				else {
					return false;
				}
			}
			else {
				return true;
			}
		}
		else {
			return false;
		}
	}

	/**
	 * Calculates the number of days in the specified month for a given year.
	 *
	 * @param month The one-based month.
	 * @param year The year.
	 * @return The number of days in the month.
	 */
	static uint16_t YearDaysInMonth(uint8_t month, uint16_t year) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return YearDaysBeforeOrAfterMonth(month, false) - YearDaysBeforeOrAfterMonth(month, true) + (month == 2 && LeapYear(year) ? 1 : 0);
	}

	/**
	 * Returns the number of days in a given year, including the additional day for leap years.
	 *
	 * @param year The year.
	 * @return The number of days for that year.
	 */
	static uint16_t DaysInYear(uint16_t year) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return LeapYear(year) ? 366 : 365;
	}

	/**
	 * Creates a date and time from the specified year, month, and day.
	 *
	 * @param year The year.
	 * @param month The one-based month.
	 * @param day The one-based day.
	 * @return The date time corresponding to the specified year, month, and day.
	 */
	static DateTime FromYearMonthDay(uint16_t year, uint8_t month, uint8_t day) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint64_t days = 0;

		for (uint16_t y = 0; y < year; y++) {
			days += DaysInYear(y);
		}

		days += YearDaysBeforeOrAfterMonth(month, true);

		if (month > 2 && LeapYear(year)) {
			days++;
		}

		days += day - 1;
		return DateTime(TimeSpan::FromDays(days));
	}

	/**
	 * Determines the year, month, and day from this date and time.
	 *
	 * @param year Will be set to the year.
	 * @param month Will be set to the one-based month.
	 * @param day Will be set to the one-based day.
	 */
	void ToYearMonthDay(uint16_t& year, uint8_t& month, uint8_t& day) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint16_t temp;
		uint64_t days = (uint64_t) _timeSpan.TotalDays();

		for (year = 0; days >= (temp = DaysInYear(year)); year++) {
			days -= temp;
		}

		for (month = 1; (temp = YearDaysInMonth(month, year)) <= days; month++) {
			days -= temp;
		}

		day = (uint8_t) (days + 1);
	}

public:

	/**
	 * Creates a new date and time corresponding to January 1, 0000.
	 */
	DateTime() : _timeSpan() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Returns the year component of the date.
	 *
	 * @return The year component.
	 */
	uint16_t Year() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint16_t year;
		uint8_t month;
		uint8_t day;

		ToYearMonthDay(year, month, day);
		return year;
	}

	/**
	 * Returns the month component of the date.
	 *
	 * @return The month component.
	 */
	uint8_t Month() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint16_t year;
		uint8_t month;
		uint8_t day;

		ToYearMonthDay(year, month, day);
		return month;
	}

	/**
	 * Returns the day component of the date.
	 *
	 * @return The day component.
	 */
	uint8_t Day() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint16_t year;
		uint8_t month;
		uint8_t day;

		ToYearMonthDay(year, month, day);
		return day;
	}

	/**
	 * Returns the hour component of the time.
	 *
	 * @return The hour component.
	 */
	int8_t Hour() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan.Hours();
	}

	/**
	 * Returns the minute component of the time.
	 *
	 * @return The minute component.
	 */
	int8_t Minute() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan.Minutes();
	}

	/**
	 * Returns the second component of the time.
	 *
	 * @return The second component.
	 */
	int8_t Second() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan.Seconds();
	}

	/**
	 * Returns the millisecond component of the time.
	 *
	 * @return The millisecond component.
	 */
	int16_t Millisecond() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan.Milliseconds();
	}

	/**
	 * Prints the string representation of the date and time to the output stream.
	 *
	 * @param stream The stream to print to.
	 * @param that The date and time to print.
	 * @return The stream that was printed to.
	 */
	friend std::ostream& operator << (std::ostream& stream, const DateTime& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return stream << std::setw(4) << std::setfill('0') << (int) that.Year()
			<< "-" << std::setw(2) << (int) that.Month()
			<< "-" << std::setw(2) << (int) that.Day()
			<< "T" << std::setw(2) << (int) that.Hour()
			<< ":" << std::setw(2) << (int) that.Minute()
			<< ":" << std::setw(2) << (int) that.Second()
			<< "." << std::setw(3) << (int) that.Millisecond();
	}

	/**
	 * Compares this date and time to another date and time.
	 *
	 * @param that The date and time to compare to.
	 * @return True if the dates and times are considered equal, false otherwise.
	 */
	bool operator == (const DateTime& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan == that._timeSpan;
	}

	/**
	 * Compares this date and time to another date and time.
	 *
	 * @param that The date and time to compare to.
	 * @return True if the dates and times are not considered equal, false otherwise.
	 */
	bool operator != (const DateTime& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan != that._timeSpan;
	}

	/**
	 * Compares this date and time to another date and time.
	 *
	 * @param that The date and time to compare to.
	 * @return True if this date and time is considered less than that date and time, false otherwise.
	 */
	bool operator < (const DateTime& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan < that._timeSpan;
	}

	/**
	 * Compares this date and time to another date and time.
	 *
	 * @param that The date and time to compare to.
	 * @return True if this date and time is considered less than or equal to that date and time, false otherwise.
	 */
	bool operator <= (const DateTime& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan <= that._timeSpan;
	}

	/**
	 * Compares this date and time to another date and time.
	 *
	 * @param that The date and time to compare to.
	 * @return True if this date and time is considered greater than that date and time, false otherwise.
	 */
	bool operator > (const DateTime& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan > that._timeSpan;
	}

	/**
	 * Compares this date and time to another date and time.
	 *
	 * @param that The date and time to compare to.
	 * @return True if this date and time is considered greater than or equal to that date and time, false otherwise.
	 */
	bool operator >= (const DateTime& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan >= that._timeSpan;
	}

	/**
	 * Returns a new date and time equal to this date and time increased by a time span.
	 *
	 * @param that The time span to add.
	 * @return A date and time matching the sum of this date and time and that time span.
	 */
	DateTime operator + (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return DateTime(_timeSpan + that);
	}

	/**
	 * Increases the date and time by a time span.
	 *
	 * @param that The time span to add.
	 * @return A reference to this increased date and time.
	 */
	DateTime& operator += (const TimeSpan& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_timeSpan += that;
		return *this;
	}

	/**
	 * Returns a new date and time equal to this date and time decreased by a time span.
	 *
	 * @param that The time span to subtract.
	 * @return A date and time matching the difference of this date and time and that time span.
	 */
	DateTime operator - (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return DateTime(_timeSpan - that);
	}

	/**
	 * Decreases the date and time by a time span.
	 *
	 * @param that The time span to subtract.
	 * @return A reference to this decreased date and time.
	 */
	DateTime& operator -= (const TimeSpan& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_timeSpan -= that;
		return *this;
	}

	/**
	 * Returns the time span separating two dates and times.
	 *
	 * @param that The date and time to subtract.
	 * @return A time span with duration matching the difference between the two dates and times.
	 */
	TimeSpan operator - (const DateTime& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _timeSpan - that._timeSpan;
	}

	/**
	 * Returns the date and time representing the unix epoch January 1, 1970.
	 *
	 * @return The unix epoch.
	 */
	static DateTime Epoch() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static DateTime epoch = DateTime::FromYearMonthDay(1970, 1, 1);
		return epoch;
	}

	/**
	 * Returns the date and time representing the current UTC time on the system.
	 *
	 * @return The current time in UTC.
	 */
	static DateTime Utc() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return Epoch() + TimeSpan::FromSeconds(tv.tv_sec) + TimeSpan::FromMilliseconds(tv.tv_usec / 1000);
	}

	/**
	 * Returns the date and time representing the current local time on the system.
	 *
	 * @return The current time including timezone and daylight savings time.
	 */
	static DateTime Local() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);

		return Epoch() + TimeSpan::FromSeconds(tv.tv_sec) + TimeSpan::FromMilliseconds(tv.tv_usec / 1000) - TimeSpan::FromMinutes(tz.tz_minuteswest);
	}

	/**
	 * Performs unit testing on functions in this class to ensure expected operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		assert((DateTime::Epoch() + TimeSpan::FromSeconds(67221446400LL)).Year() == 4100);
		assert((DateTime::Epoch() + TimeSpan::FromSeconds(67221446400LL)).Month() == 3);
		assert((DateTime::Epoch() + TimeSpan::FromSeconds(67221446400LL)).Day() == 1);
		assert((DateTime::FromYearMonthDay(4100, 3, 1) - Epoch()).TotalSeconds() == 67221446400LL);
	}
};

}

#endif /* DATETIME_HPP_ */
