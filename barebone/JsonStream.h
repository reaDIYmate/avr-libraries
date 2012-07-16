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
#ifndef JSON_STREAM_H
#define JSON_STREAM_H
/**
 * \file
 * \brief JsonStream class.
 */
#include <BufferedStream.h>
//------------------------------------------------------------------------------
/**
 * \class JsonStream
 * \brief JSON parsing helper class.
 */
class JsonStream : public BufferedStream {
public:
    /**
     * Construct an instance of JsonStream.
     *
     * \param[in] buffer Buffer to use for storing and parsing JSON data.
     * \param[in] bufferSize Size of the buffer;
     */
    JsonStream(char* buffer, size_t bufferSize = 0) :
        BufferedStream(buffer, bufferSize) {}
    virtual int read();
    int getIntegerByName(const char* key);
    int getIntegerByName_P(PGM_P key);
    int getObjectStringByName(const char* key, char* buffer, size_t length);
    int getObjectStringByName_P(PGM_P key, char* buffer, size_t length);
    int getStringByName(const char* key, char* buffer, size_t length);
    int getStringByName_P(PGM_P key, char* buffer, size_t length);
};

#endif // JSON_STREAM_H
