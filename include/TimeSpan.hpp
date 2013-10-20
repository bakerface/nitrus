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

#ifndef TIMESPAN_HPP_
#define TIMESPAN_HPP_

#include <assert.h>
#include <stdint.h>

#include "StackTrace.hpp"

namespace nitrus {

/**
 * A class that provides functionality for determining a distance between two points in time.
 */
class TimeSpan {
private:
	int64_t _milliseconds;

private:

	/**
	 * Creates a new time span with duration specified as an offset in milliseconds.
	 *
	 * @param milliseconds The duration in milliseconds.
	 */
	TimeSpan(int64_t milliseconds) : _milliseconds(milliseconds) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

public:

	/**
	 * Creates a new time span with a duration of zero milliseconds.
	 * This default constructor and TimeSpan::Zero() are synonymous.
	 */
	TimeSpan() : _milliseconds(0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Returns the total time span duration represented in both whole and fractional milliseconds.
	 *
	 * @return The total duration in milliseconds.
	 */
	double TotalMilliseconds() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return (double) _milliseconds;
	}

	/**
	 * Returns the total time span duration represented in both whole and fractional seconds.
	 *
	 * @return The total duration in seconds.
	 */
	double TotalSeconds() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TotalMilliseconds() / 1000;
	}

	/**
	 * Returns the total time span duration represented in both whole and fractional minutes.
	 *
	 * @return The total duration in minutes.
	 */
	double TotalMinutes() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TotalSeconds() / 60;
	}

	/**
	 * Returns the total time span duration represented in both whole and fractional hours.
	 *
	 * @return The total duration in hours.
	 */
	double TotalHours() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TotalMinutes() / 60;
	}

	/**
	 * Returns the total time span duration represented in both whole and fractional days.
	 *
	 * @return The total duration in days.
	 */
	double TotalDays() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TotalHours() / 24;
	}

	/**
	 * Returns the total time span duration represented in both whole and fractional weeks.
	 *
	 * @return The total duration in weeks.
	 */
	double TotalWeeks() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TotalDays() / 7;
	}

	/**
	 * Returns the milliseconds component of the time span duration.
	 *
	 * @return The millisecond component.
	 */
	int16_t Milliseconds() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return (int16_t) (_milliseconds % 1000);
	}

	/**
	 * Returns the seconds component of the time span duration.
	 *
	 * @return The seconds component.
	 */
	int8_t Seconds() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return (int8_t) ((_milliseconds / 1000) % 60);
	}

	/**
	 * Returns the minutes component of the time span duration.
	 *
	 * @return The minutes component.
	 */
	int8_t Minutes() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return (int8_t) ((_milliseconds / 1000 / 60) % 60);
	}

	/**
	 * Returns the hours component of the time span duration.
	 *
	 * @return The hours component.
	 */
	int8_t Hours() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return (int8_t) ((_milliseconds / 1000 / 60 / 60) % 24);
	}

	/**
	 * Returns the days component of the time span duration.
	 *
	 * @return The days component.
	 */
	int8_t Days() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return (int8_t) ((_milliseconds / 1000 / 60 / 60 / 24) % 7);
	}

	/**
	 * Returns the weeks component of the time span duration.
	 *
	 * @return The weeks component.
	 */
	int64_t Weeks() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return (int64_t) (_milliseconds / 1000 / 60 / 60 / 24 / 7);
	}

	/**
	 * Compares this time span to another time span.
	 *
	 * @param that The time span to compare to.
	 * @return True if the time spans are considered equal, false otherwise.
	 */
	bool operator == (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _milliseconds == that._milliseconds;
	}

	/**
	 * Compares this time span to another time span.
	 *
	 * @param that The time span to compare to.
	 * @return True if the time spans are not considered equal, false otherwise.
	 */
	bool operator != (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _milliseconds != that._milliseconds;
	}

	/**
	 * Compares this time span to another time span.
	 *
	 * @param that The time span to compare to.
	 * @return True if this time span is considered less than that time span, false otherwise.
	 */
	bool operator < (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _milliseconds < that._milliseconds;
	}

	/**
	 * Compares this time span to another time span.
	 *
	 * @param that The time span to compare to.
	 * @return True if this time span is considered less than or equal to that time span, false otherwise.
	 */
	bool operator <= (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _milliseconds <= that._milliseconds;
	}

	/**
	 * Compares this time span to another time span.
	 *
	 * @param that The time span to compare to.
	 * @return True if this time span is considered greater than that time span, false otherwise.
	 */
	bool operator > (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _milliseconds > that._milliseconds;
	}

	/**
	 * Compares this time span to another time span.
	 *
	 * @param that The time span to compare to.
	 * @return True if this time span is considered greater than or equal to that time span, false otherwise.
	 */
	bool operator >= (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _milliseconds >= that._milliseconds;
	}

	/**
	 * Combines the two time spans into a single time span with duration matching the sum of this time span and that time span.
	 *
	 * @param that The time span to add.
	 * @return A time span with duration matching the sum of this time span and that time span.
	 */
	TimeSpan operator + (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TimeSpan(_milliseconds + that._milliseconds);
	}

	/**
	 * Extends this time span by the duration of another time span.
	 *
	 * @param that The time span to add.
	 * @return A reference to this time span.
	 */
	TimeSpan& operator += (const TimeSpan& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_milliseconds += that._milliseconds;
		return *this;
	}

	/**
	 * Combines the two time spans into a single time span with duration matching the difference of this time span and that time span.
	 *
	 * @param that The time span to subtract.
	 * @return A reference to this time span.
	 */
	TimeSpan operator - (const TimeSpan& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TimeSpan(_milliseconds - that._milliseconds);
	}

	/**
	 * Reduces this time span by the duration of another time span.
	 *
	 * @param that The time span to subtract.
	 * @return A reference to this time span.
	 */
	TimeSpan& operator -= (const TimeSpan& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_milliseconds -= that._milliseconds;
		return *this;
	}

	/**
	 * Creates a new time span from the specified number of whole and fractional milliseconds.
	 *
	 * @param milliseconds The duration in milliseconds.
	 * @return A time span with the specified duration.
	 */
	static TimeSpan FromMilliseconds(double milliseconds) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TimeSpan((int64_t) milliseconds);
	}

	/**
	 * Creates a new time span from the specified number of whole and fractional seconds.
	 *
	 * @param seconds The duration in seconds.
	 * @return A time span with the specified duration.
	 */
	static TimeSpan FromSeconds(double seconds) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return FromMilliseconds(seconds * 1000);
	}

	/**
	 * Creates a new time span from the specified number of whole and fractional minutes.
	 *
	 * @param minutes The duration in minutes.
	 * @return A time span with the specified duration.
	 */
	static TimeSpan FromMinutes(double minutes) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return FromSeconds(minutes * 60);
	}

	/**
	 * Creates a new time span from the specified number of whole and fractional hours.
	 *
	 * @param hours The duration in hours.
	 * @return A time span with the specified duration.
	 */
	static TimeSpan FromHours(double hours) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return FromMinutes(hours * 60);
	}

	/**
	 * Creates a new time span from the specified number of whole and fractional days.
	 *
	 * @param days The duration in days.
	 * @return A time span with the specified duration.
	 */
	static TimeSpan FromDays(double days) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return FromHours(days * 24);
	}

	/**
	 * Creates a new time span from the specified number of whole and fractional weeks.
	 *
	 * @param weeks The duration in weeks.
	 * @return A time span with the specified duration.
	 */
	static TimeSpan FromWeeks(double weeks) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return FromDays(weeks * 7);
	}

	/**
	 * Creates a new time span with zero duration.
	 *
	 * @return A time span with zero duration.
	 */
	static TimeSpan Zero() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static TimeSpan zero = TimeSpan();
		return zero;
	}

	/**
	 * Represents the maximum allowed time span.
	 *
	 * @return A time span with infinite duration.
	 */
	static TimeSpan Infinite() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static TimeSpan infinite = TimeSpan(0x7fffffffffffffffLL);
		return infinite;
	}

	/**
	 * Performs unit testing on functions in this class to ensure expected operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		assert(TimeSpan() == TimeSpan::Zero());
		assert(TimeSpan::Zero().Milliseconds() == 0);
		assert(TimeSpan::FromMilliseconds(TimeSpan::FromSeconds(2.514).TotalMilliseconds()).TotalSeconds() == 2.514);
		assert(TimeSpan::FromHours(12) == TimeSpan::FromDays(0.5));
		assert(TimeSpan::FromMinutes(1) < TimeSpan::FromSeconds(75));
		assert((TimeSpan::FromWeeks(1) - TimeSpan::FromDays(2)).TotalDays() == 5);
		assert(TimeSpan::FromDays(2) + TimeSpan::FromDays(5) == TimeSpan::FromWeeks(1));
		assert(TimeSpan::FromDays(10.5).Weeks() == 1);
		assert(TimeSpan::FromDays(10.5).Days() == 3);
		assert(TimeSpan::FromDays(10.5).Hours() == 12);
		assert(TimeSpan::FromMilliseconds(3999).Seconds() == 3);
		assert(TimeSpan::FromMilliseconds(3999).Milliseconds() == 999);
		assert(TimeSpan::FromMilliseconds(-3999).Seconds() == -3);
		assert(TimeSpan::FromMilliseconds(-3999).Milliseconds() == -999);
	}
};

}

#endif /* TIMESPAN_HPP_ */
