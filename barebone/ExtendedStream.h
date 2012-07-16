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
#ifndef EXTENDED_STREAM_H
#define EXTENDED_STREAM_H
/**
 * \file
 * \brief ExtendedStream class which replaces the Stream Arduino core.
 */
#include <Arduino.h>
#include <Print.h>
//------------------------------------------------------------------------------
/**
 * \class ExtendedStream
 * \brief Replaces the Stream class and expands its parsing functionnality.
 */
class ExtendedStream : public Print {
public:
    /**
     * Construct an instance of ExtendedStream
     *
     * \param[in] timeout The timeout value for reading data (in milliseconds).
     */
    explicit ExtendedStream(uint32_t timeout) : timeout_(timeout) {}
    /**
     * Check the number of available characters.
     *
     * \return The number of available characters is returned.
     */
    virtual int available() = 0;
    /**
     * Read one character from the stream.
     *
     * \return If a valid character is read its value is returned, otherwise the
     * value -1 is returned.
     */
    virtual int read() = 0;
    bool find(char target);
    bool find(const char* target);
    bool find_P(PGM_P target);
    bool findUntil(char* target, char* terminator);
    bool findUntil_P(PGM_P target, PGM_P terminator);
    int readBytes(char* buffer, size_t length);
    int readBytesUntil(char terminator, char* buffer, size_t length);
    int readBytesUntil(const char* terminator, char* buffer, size_t length);
    int readBytesUntil_P(PGM_P terminator, char* buffer, size_t length);
    size_t write_P(PGM_P source);
//------------------------------------------------------------------------------
protected:
    int timedRead();
//------------------------------------------------------------------------------
private:
    /**
     * Number of milliseconds to wait for the next char before aborting timed
     * reads.
     */
    uint32_t timeout_;
};

#endif // EXTENDED_STREAM_H
