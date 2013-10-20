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

#ifndef BASE64_HPP_
#define BASE64_HPP_

#include "../StackTrace.hpp"

#include <assert.h>
#include <stdint.h>

namespace nitrus {

/**
 * A class that encapsulates functionality for base 64 encoding and decoding strings.
 */
class Base64 {
public:

	/**
	 * Encodes a string of data.
	 *
	 * @param value The string to encode.
	 * @return The encoded string.
	 */
	static std::string Encode(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static const char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		static unsigned char modulus[] = { 0, 2, 1 };

		std::string encoded;
		size_t length = value.length();

		for (size_t i = 0; i < length; ) {
			uint32_t octet_a = i < length ? value.at(i++) : 0;
			uint32_t octet_b = i < length ? value.at(i++) : 0;
			uint32_t octet_c = i < length ? value.at(i++) : 0;
			uint32_t abc = (octet_a << 16) + (octet_b << 8) + octet_c;

			encoded += table[(abc >> 18) & 0x3F];
			encoded += table[(abc >> 12) & 0x3F];
			encoded += table[(abc >> 6) & 0x3F];
			encoded += table[abc & 0x3F];
		}

		for (size_t i = 0; i < modulus[length % 3]; i++) {
			encoded += '=';
		}

		return encoded;
	}

	/**
	 * Decodes a string of data.
	 *
	 * @param value The string to decode.
	 * @return The decoded string.
	 */
	static std::string Decode(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return "";
	}

	/**
	 * Performs unit testing on functions in this class to ensure expected operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		assert(Base64::Encode("hello world!") == "aGVsbG8gd29ybGQh");
		assert(Base64::Decode("aGVsbG8gd29ybGQh") == "hello world!");
	}
};

}

#endif
