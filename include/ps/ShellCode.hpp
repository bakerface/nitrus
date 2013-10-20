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

#ifndef SHELLCODE_HPP_
#define SHELLCODE_HPP_

#include "../StackTrace.hpp"

#include <stdint.h>

namespace nitrus {

class ShellCode {
public:
	class x86 {
	public:
		class Register {
		public:
			enum Type {
				EAX,
				ECX,
				EDX,
				EBX,
				ESP,
				EBP,
				ESI,
				EDI
			};
		};
	};

private:
	std::string _data;

	ShellCode& Append(char value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_data += value;
		return *this;
	}

	ShellCode& Append(uint32_t value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char) value)
			.Append((char) (value >> 8))
			.Append((char) (value >> 16))
			.Append((char) (value >> 24));
	}

	ShellCode& Append(char value, x86::Register::Type offset) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char)(value + (char) offset));
	}

public:
	ShellCode() : _data() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	const std::string& ToString() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _data;
	}

	ShellCode& Push(x86::Register::Type value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char) 0x50, value);
	}

	ShellCode& Push(uint32_t value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char) 0x68).Append(value);
	}

	ShellCode& Pop(x86::Register::Type value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char) 0x58, value);
	}

	ShellCode& Move(x86::Register::Type destination, uint32_t value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char) 0xB8, destination).Append(value);
	}

	ShellCode& Call(x86::Register::Type value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char) 0xFF).Append((char) 0xD0, value);
	}

	ShellCode& Return() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char) 0xC3);
	}

	ShellCode& Leave() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Append((char) 0xC9);
	}

	virtual ~ShellCode() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

}

#endif /* SHELLCODE_HPP_ */
