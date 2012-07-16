/* reaDIYmate AVR library
 * Written by Pierre Bouchet
 * Copyright (C) 2011-2012 reaDIYmate
 *
 * This file is part of the reaDIYmate library.
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <BufferedStream.h>
//------------------------------------------------------------------------------
/**
 * Construct an instance of BufferedStream.
 *
 * \param[in] buffer The Buffer to read data from.
 * \param[in] bufferSize The size of the buffer.
 */
BufferedStream::BufferedStream(char* buffer, size_t bufferSize) :
    ExtendedStream(0),
    buffer_(buffer),
    bufferSize_(bufferSize),
    index_(0)
{
	if (bufferSize == 0)
		bufferSize_ = strlen(buffer);
}
//------------------------------------------------------------------------------
/**
 * Look for unread bytes in the buffer.
 *
 * \return The value returned is the number of available bytes.
 */
int BufferedStream::available() {
    return bufferSize_ - index_;
}
//------------------------------------------------------------------------------
/**
 * Read one byte from the buffer.
 *
 * \return The received byte or -1 if a timeout occurs.
 */
int BufferedStream::read() {
    return (available() > 0) ? buffer_[index_++] : -1;
}
//------------------------------------------------------------------------------
/** Write a byte to the buffer */
size_t BufferedStream::write(uint8_t c) {
    return 0;
}
