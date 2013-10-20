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

#ifndef PROCESS_HPP_
#define PROCESS_HPP_

#include "../StackTrace.hpp"
#include "../String.hpp"

#include <stdint.h>
#include <vector>

#ifdef _WIN32
# include <windows.h>
# include <psapi.h>
# include <tlhelp32.h>
# pragma comment(lib, "user32")
# pragma comment(lib, "psapi")

# define process_handle HANDLE
# define process_open(id) OpenProcess(PROCESS_ALL_ACCESS, FALSE, id)
# define process_close(handle) CloseHandle(handle)

bool process_read(process_handle handle, uint32_t address, void* buffer, size_t count) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	SIZE_T size;
	return ReadProcessMemory(handle, (void*) address, buffer, count, &size) && size == count;
}

bool process_write(process_handle handle, uint32_t address, const void* buffer, size_t count) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	SIZE_T size;
	return WriteProcessMemory(handle, (void*) address, buffer, count, &size) && size == count;
}

bool process_allocate(process_handle handle, size_t size, uint32_t* address) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	*address = (uint32_t) VirtualAllocEx(handle, NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return *address != NULL;
}

bool process_free(process_handle handle, uint32_t address) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return VirtualFreeEx(handle, (void*) address, 0, MEM_RELEASE);
}

uint32_t process_thread(process_handle handle, uint32_t address, uint32_t parameter) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return (uint32_t) CreateRemoteThread(handle, NULL, NULL, (LPTHREAD_START_ROUTINE) address, (void*) parameter, NULL, NULL);
}

bool process_id_from_window(const char* name, uint32_t* id) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	DWORD pid;
	HWND window = FindWindow(NULL, name);

	if (window == NULL) {
		return false;
	}

	GetWindowThreadProcessId(window, &pid);
	*id = pid;

	return true;
}

bool process_id_from_executable(const char* name, uint32_t* id) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return false;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);

	for (bool success = Process32First(hSnapshot, &pe); success; success = Process32Next(hSnapshot, &pe)) {
		if (stricmp(pe.szExeFile, name) == 0) {
			*id = pe.th32ProcessID;
			return true;
		}
	}

	return false;
}

bool process_module_address(process_handle handle, const char* module, uint32_t* address) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	DWORD size;
	char name[256];

	EnumProcessModules(handle, NULL, 0, &size);
	std::vector<HMODULE> modules(size / sizeof(HMODULE));

	if (EnumProcessModules(handle, &modules[0], modules.size(), &size)) {
		for (uint32_t i = 0; i < modules.size(); i++) {
			if (GetModuleBaseName(handle, modules[i], name, sizeof(name))) {
				if (nitrus::String::ToLowerCase(module) == nitrus::String::ToLowerCase(name)) {
					*address = (uint32_t) modules[i];
					return true;
				}
			}
		}
	}

	return false;
}

#else
# define process_handle void*
# define process_open(id) false
# define process_close(handle) false

bool process_read(process_handle handle, uint32_t address, void* buffer, size_t count) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return false;
}

bool process_write(process_handle handle, uint32_t address, const void* buffer, size_t count) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return false;
}

bool process_allocate(process_handle handle, size_t size, uint32_t* address) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return false;
}

bool process_free(process_handle handle, uint32_t address) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return false;
}

uint32_t process_thread(process_handle handle, uint32_t address, uint32_t parameter) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return 0;
}

bool process_id_from_window(const char* name, uint32_t* id) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return true;
}

bool process_id_from_executable(const char* name, uint32_t* id) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return false;
}

bool process_module_address(process_handle handle, const char* module, uint32_t* address) { nitrus::StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return false;
}

#endif

namespace nitrus {

/**
 * A class that provides functionality for manipulating a process.
 */
class Process {
public:

	/**
	 * A class that encapsulates an exception when an invalid process handle is returned.
	 */
	class InvalidHandleException : public std::runtime_error {
	public:

		/**
		 * Creates a new instance of the exception.
		 */
		InvalidHandleException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this instance of the exception.
		 */
		virtual ~InvalidHandleException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when a process could not be found.
	 */
	class ProcessNotFoundException : public std::runtime_error {
	public:

		/**
		 * Creates a new instance of the exception.
		 */
		ProcessNotFoundException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this instance of the exception.
		 */
		virtual ~ProcessNotFoundException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when a module could not be found.
	 */
	class ModuleNotFoundException : public std::runtime_error {
	public:

		/**
		 * Creates a new instance of the exception.
		 */
		ModuleNotFoundException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this instance of the exception.
		 */
		virtual ~ModuleNotFoundException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when attempting to read from a memory address.
	 */
	class ReadException : public std::runtime_error {
	public:

		/**
		 * Creates a new instance of the exception.
		 */
		ReadException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this instance of the exception.
		 */
		virtual ~ReadException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when attempting to write to a memory address.
	 */
	class WriteException : public std::runtime_error {
	public:

		/**
		 * Creates a new instance of the exception.
		 */
		WriteException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this instance of the exception.
		 */
		virtual ~WriteException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when attempting to allocate memory.
	 */
	class AllocationException : public std::runtime_error {
	public:

		/**
		 * Creates a new instance of the exception.
		 */
		AllocationException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this instance of the exception.
		 */
		virtual ~AllocationException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when attempting to free memory.
	 */
	class FreeException : public std::runtime_error {
	public:

		/**
		 * Creates a new instance of the exception.
		 */
		FreeException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this instance of the exception.
		 */
		virtual ~FreeException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

private:
	process_handle _handle;

public:

	/**
	 * Determines the unique process identifier from the specified window name.
	 *
	 * @param name The name of the window.
	 * @return The process id.
	 */
	static uint32_t GetIdFromWindow(const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint32_t id;

		if (process_id_from_window(name.c_str(), &id) == false) {
			throw ProcessNotFoundException();
		}

		return id;
	}

	/**
	 * Determines the unique process identifier from the specified process name.
	 *
	 * @param name The name of the process.
	 * @return The process id.
	 */
	static uint32_t GetIdFromName(const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint32_t id;

		if (process_id_from_executable(name.c_str(), &id) == false) {
			throw ProcessNotFoundException();
		}

		return id;
	}

	/**
	 * Creates a new instance of the process manager.
	 *
	 * @param id The process id of the process to manage.
	 */
	Process(uint32_t id) : _handle(process_open(id)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_handle == NULL) {
			throw InvalidHandleException();
		}
	}

	/**
	 * Reads the specified number of bytes from memory uint32_to the buffer.
	 * If all of the bytes could not be read, a ReadException is thrown.
	 *
	 * @param address The starting address to read from.
	 * @param buffer The destination buffer to save uint32_to.
	 * @param size The number of bytes to read.
	 */
	void Read(uint32_t address, void* buffer, size_t size) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (process_read(_handle, address, buffer, size) == false) {
			throw ReadException();
		}
	}

	/**
	 * Reads a value from memory.
	 * If all of the bytes could not be read, a ReadException is thrown.
	 *
	 * @param address The starting address to read from.
	 * @param size The number of bytes to read.
	 * @return The value read.
	 */
	std::string Read(uint32_t address, size_t size) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::vector<char> buffer(size);
		Read(address, &buffer[0], size);
		return std::string(&buffer[0], size);
	}

	/**
	 * Reads a value from memory.
	 * If all of the bytes could not be read, a ReadException is thrown.
	 *
	 * @param address The starting address to read from.
	 * @return The value read.
	 */
	template <typename T> T Read(uint32_t address) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		T value;
		Read(address, (void*) &value, sizeof(value));
		return value;
	}

	/**
	 * Writes a value to memory.
	 * If all of the bytes could not be written, a WriteException is thrown.
	 *
	 * @param address The starting address to write to.
	 * @param value The value to write.
	 */
	template <typename T> void Write(uint32_t address, T value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Write(address, (void*) &value, sizeof(value));
	}

	/**
	 * Writes a value to memory.
	 * If all of the bytes could not be written, a WriteException is thrown.
	 *
	 * @param address The starting address to read from.
	 * @param value The value to write.
	 */
	void Write(uint32_t address, const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Write(address, value.data(), value.size());
	}

	/**
	 * Writes the specified number of bytes from buffer uint32_to memory.
	 * If all of the bytes could not be written, a WriteException is thrown.
	 *
	 * @param address The starting address to write to.
	 * @param buffer The source buffer to write from.
	 * @param size The number of bytes to write.
	 */
	void Write(uint32_t address, const void* buffer, size_t size) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (process_write(_handle, address, buffer, size) == false) {
			throw WriteException();
		}
	}

	/**
	 * Allocates and reserves a chunk of memory.
	 * If the memory could not be allocated, an AllocationException is thrown.
	 *
	 * @param size The number of bytes to reserve.
	 * @return The address of the allocated memory.
	 */
	uint32_t Allocate(size_t size) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint32_t address;

		if (process_allocate(_handle, size, &address) == false) {
			throw AllocationException();
		}

		return address;
	}

	/**
	 * Frees a chunk of memory.
	 * If the memory could not be freed, a FreeException is thrown.
	 *
	 * @param address The address of the previously allocated memory.
	 */
	void Free(uint32_t address) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (process_free(_handle, address) == false) {
			throw FreeException();
		}
	}

	/**
	 * Creates a thread to execute code in the process.
	 *
	 * @param address The address to start executing at.
	 * @param parameter The parameter to pass to the thread.
	 * @return The thread handle.
	 */
	uint32_t CreateThread(uint32_t address, uint32_t parameter) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return process_thread(_handle, address, parameter);
	}

	/**
	 * Finds the base address of the specified module.
	 *
	 * @param module The case-insensitive name of the module.
	 * @return The base address of the module.
	 */
	uint32_t GetBaseAddress(const std::string& module) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		uint32_t address;

		if (process_module_address(_handle, module.c_str(), &address) == false) {
			throw ModuleNotFoundException();
		}

		return address;
	}

	/**
	 * Deletes this process manager.
	 */
	virtual ~Process() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_handle) {
			process_close(_handle);
		}
	}
};

}

#endif /* PROCESS_HPP_ */
