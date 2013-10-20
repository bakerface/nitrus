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

#ifndef STACKTRACE_HPP_
#define STACKTRACE_HPP_

#include <sstream>
#include <deque>
#include <stdexcept>
#include <string>

#ifndef __METHOD__
# ifdef __PRETTY_FUNCTION__
#  define __METHOD__ __PRETTY_FUNCTION__
# else
#  ifdef __FUNCTION__
#   define __METHOD__ __FUNCTION__
#  else
#   define __METHOD__ __func__
#  endif
# endif
#endif

namespace nitrus {

/**
 * A static class that provides functionality for stack tracing.
 * An instance of this class should be explicitly created at the beginning of every function.
 */
class StackTrace {
private:

	/**
	 * Creates a new trace from the specified stack trace.
	 *
	 * @param that The stack trace to copy.
	 */
	StackTrace(const StackTrace& that) : _function(that._function), _file(that._file), _line(that._line) {
		Collection.push_front(this);
	}

	/**
	 * Copies the specified stack trace to this stack trace.
	 *
	 * @param that The stack trace to copy.
	 * @return A reference to this stack trace.
	 */
	StackTrace& operator = (const StackTrace& that) {
		_function = that._function;
		_file = that._file;
		_line = that._line;

		return *this;
	}

public:

	/**
	 * Creates a new trace and pushes it on top of the static stack trace.
	 *
	 * @param function The function identifier. This can be either the function name or function declaration, but should always be the __METHOD__ macro.
	 * @param file The name of the source file containing the function. This can be either relative or absolute, but should always be the __FILE__ macro.
	 * @param line The line of the source file that contains the function declaration. This is one-based and should always be the __LINE__ macro.
	 */
	StackTrace(const char* function, const char* file, int line) : _function(function), _file(file), _line(line) {
		Collection.push_front(this);
	}

	/**
	 * Deletes the trace and pops it off of the static stack trace.
	 */
	virtual ~StackTrace() {
		Collection.pop_front();
	}

	/**
	 * Prints a string representation of the static stack trace to the specified output stream.
	 *
	 * @param stream The output stream to print to.
	 */
	static void Print(std::ostream& stream) {
		for (Container::const_iterator i = Collection.begin(); i != Collection.end(); i++) {
			stream << std::endl << " at " << (*i)->_function << " (" << (*i)->_file << ":" << (*i)->_line << ")";
		}
	}

	/**
	 * Prints a string representation of the current exception and static stack trace to the specified output stream.
	 *
	 * @param stream The output stream to print to.
	 */
	static void PrintException(std::ostream& stream) {
		try {
			throw; // re-throw the pending exception
		}
		catch (const std::exception& e) {
			stream << e.what();
		}
		catch (...) {
			stream << "An unhandled exception occurred";
		}

		Print(stream);
	}

	/**
	 * Returns a string representation of the static stack trace.
	 */
	static std::string ToString() {
		std::stringstream stream;
		Print(stream);
		return stream.str();
	}

	/**
	 * Returns a string representation of the current exception and static stack trace.
	 */
	static std::string ToExceptionString() {
		std::stringstream stream;
		PrintException(stream);
		return stream.str();
	}

private:
	typedef std::deque<StackTrace*> Container;
	static Container Collection;

	const char* _function;
	const char* _file;
	int _line;
};

StackTrace::Container StackTrace::Collection = StackTrace::Container();

}

#endif /* STACKTRACE_HPP_ */
