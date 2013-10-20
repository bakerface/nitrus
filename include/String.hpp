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

#ifndef STRING_HPP_
#define STRING_HPP_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <vector>

#include "StackTrace.hpp"

#ifdef _WIN32
int vasprintf(char** buffer, const char* format, va_list ap) {
	int size;

	if ((size = vsnprintf(NULL, 0, format, ap)) < 0 || buffer == NULL) {
		*buffer = NULL;
		return size;
	}

	if ((*buffer = (char *) malloc(size + 1)) == NULL) {
		*buffer = NULL;
		return -1;
	}

	return vsnprintf(*buffer, size + 1, format, ap);
}
#endif

namespace nitrus {

/**
 * A static class that provides functions for common string operations.
 */
class String {
public:

	/**
	 * An invalid format exception used when string conversions fails because the string was not properly formatted.
	 */
	class InvalidFormatException : public std::runtime_error {
	public:

		/**
		 * Creates a new instance of the invalid format exception.
		 */
		InvalidFormatException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the instance of the invalid format exception.
		 */
		virtual ~InvalidFormatException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * Converts all upper case characters in a string to lower case.
	 *
	 * @param value The string containing upper case characters.
	 * @return A new string identical to the specified string with the exception that all upper case characters are now lower case.
	 */
	static std::string ToLowerCase(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string lowercase = value;
		std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
		return lowercase;
	}

	/**
	 * Converts all lower case characters in a string to upper case.
	 *
	 * @param value The string containing lower case characters.
	 * @return A new string identical to the specified string with the exception that all lower case characters are now upper case.
	 */
	static std::string ToUpperCase(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string uppercase = value;
		std::transform(uppercase.begin(), uppercase.end(), uppercase.begin(), ::toupper);
		return uppercase;
	}

	/**
	 * Removes all leading whitespace from a string.
	 *
	 * @param value The string containing leading whitespace.
	 * @return A new string identical to the specified string with the exception that all leading whitespace has been removed.
	 */
	static std::string TrimLeft(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		size_t index = value.find_first_not_of(TrimmedCharacters);

		if (index != std::string::npos) {
			return value.substr(index);
		}

		return value;
	}

	/**
	 * Removes all trailing whitespace from a string.
	 *
	 * @param value The string containing trailing whitespace.
	 * @return A new string identical to the specified string with the exception that all trailing whitespace has been removed.
	 */
	static std::string TrimRight(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		size_t index = value.find_last_not_of(TrimmedCharacters);

		if (index != std::string::npos) {
			return value.substr(0, index + 1);
		}

		return value;
	}

	/**
	 * Removes all leading and trailing whitespace from a string.
	 *
	 * @param value The string containing leading and trailing whitespace.
	 * @return A new string identical to the specified string with the exception that all leading and trailing whitespace has been removed.
	 */
	static std::string Trim(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return TrimLeft(TrimRight(value));
	}

	/**
	 * Converts a value represented as a string to the specified type representation.
	 *
	 * @param value The value in string representation.
	 * @param modifier The optional format modifier function. This is decimal by default.
	 * @return The value in the specified representation.
	 */
	template <typename T> static T Convert(const std::string& value, std::ios_base& (*modifier) (std::ios_base&) = std::dec) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		T result;
		std::stringstream stream(value);

		if ((stream >> modifier >> result).fail()) {
			throw InvalidFormatException();
		}

		return result;
	}

	/**
	 * Converts a value represented as the specified type to the string representation.
	 *
	 * @param value The value in the specified representation.
	 * @param modifier The optional format modifier function. This is decimal by default.
	 * @return The value in string representation.
	 */
	template <typename T> static std::string Convert(const T& value, std::ios_base& (*modifier) (std::ios_base&) = std::dec) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::stringstream stream;

		if ((stream << modifier << value).fail()) {
			throw InvalidFormatException();
		}

		return stream.str();
	}

	/**
	 * Converts a set of values to a formatted string representation.
	 *
	 * @param format The format of the string representation.
	 * @param args The list of variable arguments. The argument list must have already been started using va_start. This function will end the list using va_end.
	 * @return The string representation of the values.
	 */
	static std::string Format(const char* format, va_list args) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		char* text;
		int count;
		
		count = vasprintf(&text, format, args);
		va_end(args);

		if (text == NULL) {
			throw InvalidFormatException();
		}

		std::string value = text;
		free(text);
		return value;
	}

	/**
	 * Converts a set of values to a formatted string representation.
	 *
	 * @param format The format of the string representation.
	 * @param ... The optional values.
	 * @return The string representation of the values.
	 */
	static std::string Format(const char* format, ...) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		va_list args;
		va_start(args, format);
		return Format(format, args);
	}

	/**
	 * Splits a string separated by a delimiter into individual parts.
	 *
	 * @param value The string to split.
	 * @param delimiter The delimiter that separates the individual parts.
	 * @param split The container used to store the individual parts.
	 */
	static void Split(const std::string& value, char delimiter, std::vector<std::string>& split) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::stringstream stream(value);
		std::string item;

		while(std::getline(stream, item, delimiter)) {
			split.push_back(item);
		}
	}

	/**
	 * Splits a string separated by a delimiter into individual parts.
	 *
	 * @param value The string to split.
	 * @param delimiter The delimiter that separates the individual parts.
	 * @return A container with the individual parts.
	 */
	static std::vector<std::string> Split(const std::string& value, char delimiter) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::vector<std::string> split;
		Split(value, delimiter, split);
		return split;
	}

	/**
	 * Replaces all occurrences of a string inside of a string with another string.
	 *
	 * @param value The string containing the text to replace.
	 * @param pre The text to search for and replace.
	 * @param post The text to replace with.
	 * @return A string with all of the occurrences replaced.
	 */
	static std::string Replace(const std::string& value, const std::string& pre, const std::string& post) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		size_t preLength = pre.length();
		size_t postLength = post.length();

		std::string replaced = value;

		for (size_t i = 0; i != std::string::npos; i += postLength) {
			if ((i = replaced.find(pre, i)) != std::string::npos) {
				replaced.replace(i, preLength, post);
			}
			else {
				break;
			}
		}

		return replaced;
	}

	/**
	 * Performs unit testing on functions in this class to ensure expected operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		assert(String::ToLowerCase("OK2go") == "ok2go");
		assert(String::ToUpperCase("OK2go") == "OK2GO");
		assert(String::TrimLeft("OK2go") == "OK2go");
		assert(String::TrimRight("OK2go") == "OK2go");
		assert(String::Trim("OK2go") == "OK2go");
		assert(String::TrimLeft(" \tOK2go\r\n") == "OK2go\r\n");
		assert(String::TrimRight(" \tOK2go\r\n") == " \tOK2go");
		assert(String::Trim(" \tOK2go\r\n") == "OK2go");
		assert(String::Convert<int>("123") == 123);
		assert(String::Convert<double>("123.123") == 123.123);
		assert(String::Convert<int>(123) == "123");
		assert(String::Convert<double>(123.123) == "123.123");
		assert(String::ToUpperCase(String::Convert<int>(254, std::hex)) == "FE");
		assert(String::Convert<int>("FE", std::hex) == 254);
		assert(String::Format("this %d is a %s", 41, "test") == "this 41 is a test");
		assert(String::Replace("this is a test replacement string", "is", "at") == "that at a test replacement string");
		assert(String::Replace("this is a test replacement string", "st", "_st_") == "this is a te_st_ replacement _st_ring");
	}

private:
	static std::string TrimmedCharacters;
};

std::string String::TrimmedCharacters = " \t\r\n";

}

#endif /* STRING_HPP_ */
