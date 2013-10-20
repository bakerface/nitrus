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

#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <queue>

#include "StackTrace.hpp"
#include "TimeSpan.hpp"
#include "DateTime.hpp"
#include "Delegate.hpp"

#ifdef _WIN32
# include <windows.h>
# define msleep(ms) ::Sleep(ms)
#else
# include <unistd.h>
# define msleep(ms) ::usleep((ms) * 1000);
#endif


namespace nitrus {

/**
 * A class that provides methods for creating timed events and running them in an event loop.
 */
class Thread {
private:

	/**
	 * The amount of time this thread has been idle.
	 */
	static TimeSpan Idle;

	/**
	 * The date and time this thread started running.
	 */
	static DateTime Started;

	/**
	 * A container class for a delegate that is invoked at a future time.
	 */
	class FutureEventHandler {
	protected:
		DateTime _time;
		Delegate<void ()> _delegate;

	public:

		/**
		 * Creates a new future event handler.
		 *
		 * @param time The time that the delegate should be invoked.
		 * @param delegate The delegate to invoke.
		 */
		FutureEventHandler(const DateTime& time, const Delegate<void ()>& delegate) : _time(time), _delegate(delegate) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the future event handler.
		 */
		virtual ~FutureEventHandler() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * The time that the delegate should be invoked.
		 * Since the event loop is blocking, it is not guaranteed that the delegate will be invoked exactly at that time.
		 *
		 * @return The time the delegate should be invoked.
		 */
		const DateTime& Time() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _time;
		}

		/**
		 * Invokes the delegate.
		 */
		void operator ()() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_delegate();
		}

		/**
		 * Compares this future event handler to another future event handler.
		 *
		 * @param that The event handler to compare to.
		 * @return True if this event is fired before that event, false otherwise.
		 */
		bool operator < (const FutureEventHandler& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _time < that._time;
		}

		/**
		 * Compares this future event handler to another future event handler.
		 *
		 * @param that The event handler to compare to.
		 * @return True if this event is fired after that event, false otherwise.
		 */
		bool operator > (const FutureEventHandler& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _time > that._time;
		}
	};

	typedef std::priority_queue<FutureEventHandler, std::vector<FutureEventHandler>, std::greater<FutureEventHandler> > EventQueue;
	static EventQueue FutureEvents;

public:

	/**
	 * Returns the thread utilization.
	 *
	 * @return A value in between zero and one, where zero means the thread was completely idle, and one means the thread was completely busy.
	 */
	static double Utilization() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		double duration = (DateTime::Utc() - Started).TotalMilliseconds();
		return (duration - Idle.TotalMilliseconds()) / duration;
	}

	/**
	 * Signals the current thread to stop processing for the specified time span.
	 *
	 * @param timeSpan The amount of time to stop processing.
	 */
	static void Sleep(const TimeSpan& timeSpan) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (timeSpan > TimeSpan::Zero()) {
			msleep((uint32_t) timeSpan.TotalMilliseconds());
			Idle += timeSpan;
		}
	}

	/**
	 * Schedules a delegate to be executed after the specified amount of time passed.
	 *
	 * @param timeout The duration to wait before executing the delegate.
	 * @param delegate The delegate to invoke.
	 */
	static void SetTimeout(const TimeSpan& timeout, const Delegate<void ()>& delegate) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		FutureEvents.push(FutureEventHandler(DateTime::Utc() + timeout, delegate));
	}

	/**
	 * Schedules a delegate to be executed as soon as the thread is capable.
	 *
	 * @param delegate The delegate to invoke.
	 */
	static void Invoke(const Delegate<void ()>& delegate) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Thread::SetTimeout(TimeSpan::Zero(), delegate);
	}

	/**
	 * Processes all of the scheduled delegates until there are no more to execute.
	 * Sleeps when waiting for a scheduled delegate to be ready to invoke to reduce processor usage.
	 */
	static void Run() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		while (FutureEvents.empty() == false) {
			FutureEventHandler event = FutureEvents.top();
			FutureEvents.pop();
			Sleep(event.Time() - DateTime::Utc());
			event();
		}
	}

	/**
	 * Performs unit testing on functions in this class to ensure expected operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

Thread::EventQueue Thread::FutureEvents = Thread::EventQueue();
TimeSpan Thread::Idle = TimeSpan::Zero();
DateTime Thread::Started = DateTime::Utc();

}

#endif /* THREAD_HPP_ */
