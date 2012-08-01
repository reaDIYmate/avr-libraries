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
#ifndef BUFFERED_STREAM_H
#define BUFFERED_STREAM_H
/**
 * \file
 * \brief BufferedStream class to read data from buffers.
 */
#include <ExtendedStream.h>
//------------------------------------------------------------------------------
/**
 * \class BufferedStream
 * \brief Encapsulates printing and parsing functions for buffered data.
 */
class BufferedStream : public ExtendedStream {
public:
    BufferedStream(char* buffer, size_t bufferSize = 0);
    virtual int available();
    virtual int read();
    virtual void rewind() { index_ = 0; }
    virtual size_t write(uint8_t);
//------------------------------------------------------------------------------
protected:
    /** Buffer to read data from */
    char* buffer_;
    /** Size of the buffer */
    size_t bufferSize_;
    /** Index used to browse buffer */
    uint16_t index_;
};

#endif // BUFFERED_STREAM_H
