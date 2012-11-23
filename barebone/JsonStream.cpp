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
#include <JsonStream.h>
//------------------------------------------------------------------------------
/** Size of the temporary buffer used for sscanf() */
const char ATOI_BUFFER_SIZE = 16;
//------------------------------------------------------------------------------
/**
 * Parse the integer value corresponding to the key string provided.
 *
 * \param[in] key Key string to find.
 *
 * \return If the key string is found and the associated value is an integer,
 * this integer is returned.
 */
int JsonStream::getIntegerByName(const char* key) {
    rewind();
    if (!find(key))
        return -1;
    if (!find(':'))
        return -1;
    char buffer[ATOI_BUFFER_SIZE];
    readBytesUntil('}', buffer, ATOI_BUFFER_SIZE);
    return atoi(buffer);
}
//------------------------------------------------------------------------------
/**
 * Parse the integer value corresponding to the key string provided.
 *
 * \param[in] key Key string to find.
 *
 * \return If the key string is found and the associated value is an integer,
 * this integer is returned.
 */
int JsonStream::getIntegerByName_P(PGM_P key) {
    rewind();
    if (!find_P(key))
        return -1;
    if (!find(':'))
        return -1;
    char buffer[ATOI_BUFFER_SIZE];
    readBytesUntil('}', buffer, ATOI_BUFFER_SIZE);
    return atoi(buffer);
}
//------------------------------------------------------------------------------
/**
 * Find the provided key string and write the associated object string to the
 * target buffer. This functions allows one leve of depth in the supported JSON
 * strings.
 *
 * \param[in] key Key string to find.
 * \param[out] buffer Target buffer to write the value.
 * \param[in] length Maximum length to write to the buffer.
 *
 * \return The number of characters written to the buffer is returned.
 */
int JsonStream::getObjectStringByName(const char* key, char* buffer, size_t length) {
    rewind();
    if (!find(key))
        return -1;
    if (!find(':'))
        return -1;
     if (!find('{'))
        return -1;
    return readBytesUntil('}', buffer, length);
}//------------------------------------------------------------------------------
/**
 * Find the provided key string and write the associated object string to the
 * target buffer. This functions allows one leve of depth in the supported JSON
 * strings.
 *
 * \param[in] key Key string to find.
 * \param[out] buffer Target buffer to write the value.
 * \param[in] length Maximum length to write to the buffer.
 *
 * \return The number of characters written to the buffer is returned.
 */
int JsonStream::getObjectStringByName_P(PGM_P key, char* buffer, size_t length) {
    rewind();
    if (!find_P(key))
        return -1;
    if (!find(':'))
        return -1;
     if (!find('{'))
        return -1;
    return readBytesUntil('}', buffer, length);
}
//------------------------------------------------------------------------------
/**
 * Find the provided key string and write the associated value string to the
 * target buffer.
 *
 * \param[in] key Key string to find.
 * \param[out] buffer Target buffer to write the value.
 * \param[in] length Maximum length to write to the buffer.
 *
 * \return The number of characters written to the buffer is returned.
 */
int JsonStream::getStringByName(const char* key, char* buffer, size_t length) {
    rewind();
    uint8_t len = strlen(key);

    char ch = '\0';
    while (ch != '\"') {
        if (find(key)) {
            ch = buffer_[index_-len-1];
        }
        else {
            return -1;
        }
    }
    if (!find(':'))
        return -1;
    if (!find('"'))
        return -1;
    return readBytesUntil('"', buffer, length);
}
//------------------------------------------------------------------------------
/**
 * Find the provided key string and write the associated value string to the
 * target buffer.
 *
 * \param[in] key Key string to find.
 * \param[out] buffer Target buffer to write the value.
 * \param[in] length Maximum length to write to the buffer.
 *
 * \return The number of characters written to the buffer is returned.
 */
int JsonStream::getStringByName_P(PGM_P key, char* buffer, size_t length) {
    rewind();
    if (!find_P(key))
        return -1;
    if (!find(':'))
        return -1;
    if (!find('"'))
        return -1;
    return readBytesUntil('"', buffer, length);
}
//------------------------------------------------------------------------------
/**
 * Read one byte from the buffer, unescape characters which are escaped.
 *
 * \return The received byte or -1 if a timeout occurs.
 */
int JsonStream::read() {
    int c = BufferedStream::read();
    if (c == '\\') {
        c = BufferedStream::read();
        switch (c) {
            case 'b' :
                return '\b';
            case 'f' :
                return '\f';
            case 'n' :
                return '\n';
            case 'r' :
                return '\r';
            case 't' :
                return '\t';
        }
    }
    return c;
}
