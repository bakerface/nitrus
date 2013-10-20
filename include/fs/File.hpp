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

#ifndef FILE_HPP_
#define FILE_HPP_

#include "../StackTrace.hpp"
#include "../Event.hpp"

#include <stdio.h>
#include <vector>

namespace nitrus {

/**
 * A class that provides functionality for reading and writing to files.
 */
class File {
public:

	/**
	 * The default chunk size used when reading files.
	 */
	static size_t DefaultChunkSize;

	/**
	 * A class that encapsulates an exception when attempting to open a file that does not exist.
	 */
	class FileNotFoundException : public std::runtime_error {
	public:

		/**
		 * Creates a new file not found exception.
		 */
		FileNotFoundException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this exception.
		 */
		virtual ~FileNotFoundException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates a chunk that has been read from a file.
	 */
	class ChunkReadEventArgs : public EventArgs {
	private:
		std::string _data;

	public:

		/**
		 * Creates a new chunk read event argument with the specified chunk data.
		 *
		 * @param data The chunk of the file.
		 */
		ChunkReadEventArgs(const std::string& data) : _data(data) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Returns the chunk data.
		 * The size of this chunk may not be consistent with previous chunk sizes.
		 *
		 * @return The chunk data.
		 */
		const std::string& Data() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _data;
		}

		/**
		 * Deletes this event argument.
		 */
		virtual ~ChunkReadEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ChunkReadEventArgs&> ChunkReadEventHandler;
	typedef Event<const ChunkReadEventArgs&> ChunkReadEvent;

	/**
	 * A class that encapsulates the end of a file.
	 */
	class EndOfFileEventArgs : public EventArgs {
	public:

		/**
		 * Creates a new end of file event argument.
		 */
		EndOfFileEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this event argument.
		 */
		virtual ~EndOfFileEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const EndOfFileEventArgs&> EndOfFileEventHandler;
	typedef Event<const EndOfFileEventArgs&> EndOfFileEvent;

private:

	/**
	 * A class that handles reading file contents.
	 */
	class FileReader {
	private:
		FILE* _file;
		std::vector<char> _buffer;
		ChunkReadEvent _chunkRead;
		EndOfFileEvent _endOfFile;

	private:
		template <typename Signature> friend class Delegate;

		/**
		 * Updates the file reader.
		 * This function will read the next chunk from the file.
		 * If the end of file is reached, this function will automatically dispose of this object.
		 */
		void Update() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t count = fread(&_buffer[0], 1, _buffer.size(), _file);

			if (count > 0) {
				_chunkRead(ChunkReadEventArgs(std::string(&_buffer[0], count)), this);
			}

			if (feof(_file)) {
				_endOfFile(EndOfFileEventArgs(), this);
				delete this;
			}
			else {
				Thread::Invoke(delegate(&FileReader::Update, this));
			}
		}

	public:

		/**
		 * Creates a new file reader.
		 *
		 * @param path The path of the file to read.
		 * @param bufferSize The desired size of each chunk.
		 */
		FileReader(const std::string& path, size_t bufferSize) : _file(fopen(path.c_str(), "rb")), _buffer(bufferSize), _chunkRead(), _endOfFile() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_file == NULL) {
				throw FileNotFoundException();
			}
		}
		
		/**
		 * Creates a new file reader as a copy of another.
		 *
		 * @param that The file reader to clone.
		 */
		FileReader(const FileReader& that) : _file(that._file), _buffer(that._buffer), _chunkRead(that._chunkRead), _endOfFile(that._endOfFile) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a file reader into this file reader.
		 *
		 * @param that The file reader to clone.
		 */
		FileReader& operator = (const FileReader& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_file = that._file;
			_buffer = that._buffer;
			_chunkRead = that._chunkRead;
			_endOfFile = that._endOfFile;
			
			return *this;
		}

		/**
		 * Starts the asynchronous read of the file.
		 */
		void Read() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			Thread::Invoke(delegate(&FileReader::Update, this));
		}

		/**
		 * This event is called when a chunk has been read from the file.
		 *
		 * @return The event.
		 */
		ChunkReadEvent& ChunkRead() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _chunkRead;
		}

		/**
		 * This event is called when all chunks have been read and the end of file has been reached.
		 *
		 * @return The event.
		 */
		EndOfFileEvent& EndOfFile() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _endOfFile;
		}

		/**
		 * Deletes this file reader.
		 */
		virtual ~FileReader() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_file != NULL) {
				fclose(_file);
			}
		}
	};

public:

	/**
	 * Reads a file.
	 *
	 * @param path The path of the file to read.
	 * @param chunkRead The method to invoke when a chunk has been read from the file.
	 * @param endOfFile The method to invoke when all chunks have been read from the file and the end of file has been reached.
	 * @param chunkSize The desired chunk size.
	 */
	static void Read(const std::string& path, const ChunkReadEventHandler& chunkRead = ChunkReadEventHandler::Empty(), const EndOfFileEventHandler& endOfFile = EndOfFileEventHandler::Empty(), size_t chunkSize = DefaultChunkSize) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		FileReader* reader = new FileReader(path, chunkSize);
		reader->ChunkRead() += chunkRead;
		reader->EndOfFile() += endOfFile;
		reader->Read();
	}

	/**
	 * Extracts the file extension from a file path.
	 *
	 * @param path The file path.
	 * @return The file extension (excluding the '.').
	 */
	static std::string GetExtension(const std::string& path) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		size_t index = path.find_last_of('.');

		if (index == std::string::npos || index == path.length() - 1) {
			return "";
		}

		return path.substr(index + 1);
	}
};

size_t File::DefaultChunkSize = 4096;

}

#endif /* FILE_HPP_ */
