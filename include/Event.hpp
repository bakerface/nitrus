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

#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <list>

#include "StackTrace.hpp"
#include "Delegate.hpp"

namespace nitrus {

/**
 * The base class for all event arguments.
 * Every event that is fired will have a derived instance of this class as the first parameter in the delegate.
 */
class EventArgs {
public:

	/**
	 * Creates a new set of event arguments.
	 */
	EventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Deletes the event arguments.
	 */
	virtual ~EventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Returns an empty set of event arguments.
	 *
	 * @return The empty event arguments.
	 */
	static EventArgs Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static EventArgs empty;
		return empty;
	}
};

/**
 * A helper class for all event listener delegates.
 */
template <typename EventArgs> class EventHandler : public Delegate<void (EventArgs, void*)> {
public:

	/**
	 * Creates a new event handler that does nothing when invoked.
	 */
	EventHandler() : Delegate<void (EventArgs, void*)>() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new event handler that invokes a static function.
	 *
	 * @param function The static function pointer.
	 */
	EventHandler(void (*function)(EventArgs, void*)) : Delegate<void (EventArgs, void*)>(function) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new event handler that invokes a member function.
	 *
	 * @param function The member function pointer.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> EventHandler(void (Instance::*function)(EventArgs, void*), Instance* instance) : Delegate<void (EventArgs, void*)>(function, instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new event handler that invokes a constant member function.
	 *
	 * @param function The member function pointer.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> EventHandler(void (Instance::*function)(EventArgs, void*) const, const Instance* instance) : Delegate<void (EventArgs, void*)>(function, instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Deletes the event handler.
	 */
	virtual ~EventHandler() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Returns an empty event handler that does nothing when invoked.
	 *
	 * @return The empty event handler.
	 */
	static EventHandler<EventArgs> Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static EventHandler<EventArgs> empty;
		return empty;
	}
};

/**
 * A container class for a set of event handlers.
 * When an event is invoked, all event handlers are invoked.
 */
template <typename EventArgs> class Event {
private:
	std::list<EventHandler<EventArgs> > _handlers;

public:

	/**
	 * Creates a new event with no event handlers.
	 */
	Event() : _handlers() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
    
    /**
	 * Creates a new event from another event.
	 *
	 * @param that The event to clone.
	 */
	Event(const Event<EventArgs>& that) : _handlers(that._handlers) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		
	}
    
	/**
	 * Assigns this event to a clone of the specified event.
	 *
	 * @param that The event to clone.
	 * @return This event.
	 */
	Event<EventArgs>& operator = (const Event<EventArgs>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_handlers = that._handlers;
		return *this;
	}

	/**
	 * Deletes the event.
	 */
	virtual ~Event() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Adds a new event handler to this event.
	 * The event handler will be invoked when the event is invoked.
	 * If the event handler has already been added, the handler is not added again.
	 *
	 * @param that The event handler.
	 * @return A reference to this event.
	 */
	Event<EventArgs>& operator += (const EventHandler<EventArgs>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_handlers.push_back(that);
		_handlers.unique();
		return *this;
	}

	/**
	 * Removes the event handler from this event.
	 * The event handler will no longer be invoked when the event is invoked.
	 * This method assumes that the event handler has previously been added.
	 *
	 * @param that The event handler.
	 * @return A reference to this event.
	 */
	Event<EventArgs>& operator -= (const EventHandler<EventArgs>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_handlers.remove(that);
		return *this;
	}

	/**
	 * Triggers the event.
	 * This invokes all event handlers that have been added.
	 *
	 * @param args The arguments to the event. This should be a class that derives from EventArgs.
	 * @param sender The sender of the event. This should always be the instance of the class containing this event.
	 */
	void operator ()(EventArgs args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::list<EventHandler<EventArgs> > handlers = _handlers; // prevent any potential modify-while-iterator errors

		for (typename std::list<EventHandler<EventArgs> >::iterator i = handlers.begin(); i != handlers.end(); i++) {
			(*i)(args, sender);
		}
	}

	/**
	 * Returns an empty event with no event handlers.
	 *
	 * @return An empty event.
	 */
	static Event<EventArgs> Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static Event<EventArgs> empty;
		return empty;
	}
};

template <typename EventArgs> EventHandler<EventArgs> delegate(void (*function)(EventArgs, void*)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return EventHandler<EventArgs>(function);
}

template <typename EventArgs, typename Instance> EventHandler<EventArgs> delegate(void (Instance::*function)(EventArgs, void*), Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return EventHandler<EventArgs>(function, instance);
}

template <typename EventArgs, typename Instance> EventHandler<EventArgs> delegate(void (Instance::*function)(EventArgs, void*) const, const Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return EventHandler<EventArgs>(function, instance);
}

}

#endif /* EVENT_HPP_ */
