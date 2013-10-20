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

#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include "StackTrace.hpp"
#include "Log.hpp"
#include "Random.hpp"
#include "String.hpp"
#include "TimeSpan.hpp"
#include "DateTime.hpp"
#include "Thread.hpp"

#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <map>

#ifdef _WIN32
# define SIGDEBUG SIGBREAK
#else
# define SIGDEBUG SIGUSR1
#endif

namespace nitrus {

/**
 * A static class that provides functions for controlling the flow of the application.
 */
class Application {
private:
	typedef std::map<std::string, std::string> ParameterMap;

	/**
	 * A map used to hold global application parameters.
	 */
	static ParameterMap Parameters;

	/**
	 * This function is used as the handler for all process debug signals.
	 * When a signal is received, debugging information is printed to the standard output stream.
	 *
	 * @param parameter The signal number.
	 */
	static void DebugSignalHandler(int parameter) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Log::Information("The debug signal has been caught.");
		Log::Information("Thread utilization is %3.2f%%.", Thread::Utilization() * 100);
		Log::Debug("The current stack trace is%s", StackTrace::ToString().c_str());
		assert(signal(SIGDEBUG, DebugSignalHandler) != SIG_ERR);
	}

	/**
	 * This function is used as the handler for all process segmentation fault signals.
	 * When a signal is received, a stack trace is printed to the standard error stream.
	 *
	 * @param parameter The signal number.
	 */
	static void SegmentationFaultSignalHandler(int parameter) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Log::Error("The application has encountered a segmentation fault%s", StackTrace::ToString().c_str());
	}

	/**
	 * This function is used as the handler for all process abort signals.
	 * When a signal is received, a stack trace is printed to the standard error stream.
	 *
	 * @param parameter The signal number.
	 */
	static void AbortSignalHandler(int parameter) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Log::Error("The application has been aborted%s", StackTrace::ToString().c_str());
	}

	/**
	 * This function is used as the handler for process termination.
	 * When the process is shutdown unexpectedly, a stack trace is printed to the standard error stream.
	 */
	static void TerminationHandler() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Log::Error("The application has terminated unexpectedly%s", StackTrace::ToString().c_str());
	}

	/**
	 * Adds exception handlers for common process failure signals.
	 * This will allow for unexpected failures to be caught.
	 */
	static void InstallExceptionHandlers() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		assert(signal(SIGSEGV, SegmentationFaultSignalHandler) != SIG_ERR);
		assert(signal(SIGABRT, AbortSignalHandler) != SIG_ERR);
		assert(signal(SIGDEBUG, DebugSignalHandler) != SIG_ERR);
		std::set_terminate(TerminationHandler);
	}

	/**
	 * Performs unit testing on functions in all classes to ensure expected application operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Log::UnitTest();
		Random::UnitTest();
		String::UnitTest();
		TimeSpan::UnitTest();
		DateTime::UnitTest();
		Thread::UnitTest();
	}

public:

	/**
	 * Sets a global application parameter.
	 *
	 * @param key The case-insensitive unique key of the parameter.
	 * @param value The value of the parameter.
	 */
	static void SetParameter(const std::string& key, const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Parameters[String::ToLowerCase(key)] = value;
	}

	/**
	 * Sets a global application parameter.
	 *
	 * @param key The case-insensitive unique key of the parameter.
	 * @param value The value of the parameter.
	 */
	template <typename T> static void SetParameter(const std::string& key, const T& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Parameters[String::ToLowerCase(key)] = String::Convert<T>(value);
	}

	/**
	 * Gets a global application parameter.
	 *
	 * @param key The case-insensitive unique key of the parameter.
	 * @param defaultValue The value to return if the parameter is not set.
	 * @return The value of the parameter if previously set; the default value otherwise.
	 */
	static std::string GetParameter(const std::string& key, const std::string& defaultValue = "") { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		ParameterMap::iterator i = Parameters.find(String::ToLowerCase(key));
		return i == Parameters.end() ? defaultValue : i->second;
	}

	/**
	 * Gets a global application parameter.
	 *
	 * @param key The case-insensitive unique key of the parameter.
	 * @param defaultValue The value to return if the parameter is not set.
	 * @return The value of the parameter if previously set; the default value otherwise.
	 */
	template <typename T> static T GetParameter(const std::string& key, const T& defaultValue) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		ParameterMap::iterator i = Parameters.find(String::ToLowerCase(key));
		return i == Parameters.end() ? defaultValue : String::Convert<T>(i->second);
	}

	/**
	 * Initializes and tests classes required for the application to begin.
	 * This function should be called before any other application functions.
	 *
	 * @param argc The number of elements in the second parameter.
	 * @param argv The array of arguments passed to this application from the system.
	 */
	static void Initialize(int argc, char** argv) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Application::InstallExceptionHandlers();
		Application::UnitTest();

		for (int i = 1; i < argc - 1; i += 2) {
			Application::SetParameter(argv[i], argv[i + 1]);
		}

		Application::SetParameter("--application", *argv);
		Random::Seed(Application::GetParameter<unsigned int>("--seed", (unsigned int) time(NULL)));
	}

	/**
	 * Starts running the application.
	 * This will run the thread's event loop and will return when all processing is finished.
	 * If an exception is thrown, a stack trace is printed.
	 * This function should be called at the end of all other application functions.
	 *
	 * @return EXIT_SUCCESS if the application completed successfully or EXIT_FAILURE if an error occurred.
	 */
	static int Run() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		try {
			Thread::Run();
			return EXIT_SUCCESS;
		}
		catch (...) {
			Log::Error(StackTrace::ToExceptionString());
		}

		return EXIT_FAILURE;
	}
};

Application::ParameterMap Application::Parameters = Application::ParameterMap();

}

#endif /* APPLICATION_HPP_ */
