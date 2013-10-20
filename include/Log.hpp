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

#ifndef LOG_HPP_
#define LOG_HPP_

#ifdef _WIN32
# include <windows.h>
#endif

#include <iostream>
#include <stdio.h>
#include "StackTrace.hpp"
#include "String.hpp"

namespace nitrus {

/**
 * A static class that provides functions for logging application messages.
 */
class Log {
private:

#ifdef _WIN32
	enum Color {
		Color_Black = 0,
		Color_Red = FOREGROUND_RED,
		Color_Green = FOREGROUND_GREEN,
		Color_Yellow = FOREGROUND_RED | FOREGROUND_GREEN,
		Color_Blue = FOREGROUND_BLUE,
		Color_Magenta = FOREGROUND_RED | FOREGROUND_BLUE,
		Color_Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
		Color_White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		Color_Default = Color_White
	};
#else
	enum Color {
		Color_Default = 0,
		Color_Black = 30,
		Color_Red = 31,
		Color_Green = 32,
		Color_Yellow = 33,
		Color_Blue = 34,
		Color_Magenta = 35,
		Color_Cyan = 36,
		Color_White = 37
	};
#endif

	/**
	 * Changes the foreground color of the console.
	 *
	 * @param color The foreground color.
	 */
	static void Foreground(Color color) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD) color);
#else
		printf("\x1b[%dm", color);
#endif
	}

	/**
	 * Prints text to the log file using the specified text color.
	 *
	 * @param foreground The foreground color to print the text in.
	 * @param format The format of the text.
	 * @param ... The set of optional arguments to print.
	 */
	static void Print(Color foreground, const char* format, va_list args) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Foreground(foreground);
		std::cout << String::Format(format, args) << std::endl;
		Foreground(Color_Default);
	}

	/**
	 * Prints text to the log file using the specified text color.
	 *
	 * @param foreground The foreground color to print the text in.
	 * @param value The value to print.
	 */
	template <typename T> static void Print(Color foreground, T value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Foreground(foreground);
		std::cout << value << std::endl;
		Foreground(Color_Default);
	}

public:

	/**
	 * Prints a debugging message to the log.
	 * This is the most verbose logging level.
	 * Debugging is typically disabled for production builds and used only for development and testing builds.
	 *
	 * @param format The format of the text.
	 * @param ... The set of optional arguments to print.
	 */
	static void Debug(const char* format, ...) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		va_list args;
		va_start(args, format);
		Print(Color_Cyan, format, args);
	}

	/**
	 * Prints a debugging message to the log.
	 * This is the most verbose logging level.
	 * Debugging is typically disabled for production builds and used only for development and testing builds.
	 *
	 * @param value The value to print.
	 */
	template <typename T> static void Debug(T value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Print(Color_Cyan, value);
	}

	/**
	 * Prints an informational message to the log.
	 * This is minimal log level required for development builds.
	 * This level is typically used to output information that is typical operation of the application.
	 * Noteworthly operations that are performed infrequently are usually logged at this level.
	 *
	 * @param format The format of the text.
	 * @param ... The set of optional arguments to print.
	 */
	static void Information(const char* format, ...) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		va_list args;
		va_start(args, format);
		Print(Color_Green, format, args);
	}

	/**
	 * Prints an informational message to the log.
	 * This is minimal log level required for development builds.
	 * This level is typically used to output information that is typical operation of the application.
	 * Noteworthly operations that are performed infrequently are usually logged at this level.
	 *
	 * @param value The value to print.
	 */
	template <typename T> static void Information(T value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Print(Color_Green, value);
	}

	/**
	 * Prints a warning message to the log.
	 * This log level is typically used to identify operations that are expected, but may require attention.
	 * This is minimal log level required for testing builds.
	 * An example would be if the application used a default value for a user setting, because it was not defined by the user.
	 *
	 * @param format The format of the text.
	 * @param ... The set of optional arguments to print.
	 */
	static void Warning(const char* format, ...) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		va_list args;
		va_start(args, format);
		Print(Color_Yellow, format, args);
	}

	/**
	 * Prints a warning message to the log.
	 * This log level is typically used to identify operations that are expected, but may require attention.
	 * This is minimal log level required for testing builds.
	 * An example would be if the application used a default value for a user setting, because it was not defined by the user.
	 *
	 * @param value The value to print.
	 */
	template <typename T> static void Warning(T value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Print(Color_Yellow, value);
	}

	/**
	 * Prints a fatal error to the log.
	 * This log level is required for all builds and is typically used to display conditions of application failure.
	 * These messages indicate errors that cause parts of the application to be incapable of functioning correctly.
	 *
	 * @param format The format of the text.
	 * @param ... The set of optional arguments to print.
	 */
	static void Error(const char* format, ...) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		va_list args;
		va_start(args, format);
		Print(Color_Red, format, args);
	}

	/**
	 * Prints a fatal error to the log.
	 * This log level is required for all builds and is typically used to display conditions of application failure.
	 * These messages indicate errors that cause parts of the application to be incapable of functioning correctly.
	 *
	 * @param value The value to print.
	 */
	template <typename T> static void Error(T value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Print(Color_Red, value);
	}

	/**
	 * Performs unit testing on functions in this class to ensure expected operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		// Print(Color_Black, "Color_Black");
		// Print(Color_Blue, "Color_Blue");
		// Print(Color_Cyan, "Color_Cyan");
		// Print(Color_Default, "Color_Default");
		// Print(Color_Green, "Color_Green");
		// Print(Color_Magenta, "Color_Magenta");
		// Print(Color_Red, "Color_Red");
		// Print(Color_White, "Color_White");
		// Print(Color_Yellow, "Color_Yellow");
	}
};

}

#endif /* LOG_HPP_ */
