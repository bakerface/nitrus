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

#ifndef DIRECTORY_HPP_
#define DIRECTORY_HPP_

#include "../StackTrace.hpp"
#include "../Event.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
# include <io.h>
#endif

namespace nitrus {

/**
 * A class that provides functionality for reading and writing to files.
 */
class Directory {
public:

	/**
	 * Checks if a directory exists.
	 *
	 * @param path The path to the directory.
	 * @return True if the directory exists, false otherwise.
	 */
	static bool Exists(const char* path) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (access(path, 0) == 0) {
			struct stat status;
			stat(path, &status);
			return (status.st_mode & S_IFDIR) == S_IFDIR;
		}

		return false;
	}

	/**
	 * Checks if a directory exists.
	 *
	 * @param path The path to the directory.
	 * @return True if the directory exists, false otherwise.
	 */
	static bool Exists(const std::string& path) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Exists(path.c_str());
	}
};

}

#endif /* DIRECTORY_HPP_ */
