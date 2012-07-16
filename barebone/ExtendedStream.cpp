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
#include <ExtendedStream.h>
//------------------------------------------------------------------------------
/**
 * Reads data from serial port until the target character is found.
 *
 * \param[in] target The character to look for.
 *
 * \return true if the target character is found, false if timed out.
 */
bool ExtendedStream::find(char target) {
    int c = 0;
    do {
        c = timedRead();
        if ((char)c == target)
            return true;
    } while (c >= 0);
    // if c is -1, it means a timeout occured
    return false;
}
//------------------------------------------------------------------------------
/**
 * Reads data from serial port until the target string is found.
 *
 * \param[in] target The string to find in the incoming stream.
 *
 * \return true if the target string is found, false if timed out.
 */
bool ExtendedStream::find(const char* target) {
    uint8_t targetLength = strlen(target);
    uint8_t targetIndex = 0;
    int c = 0;
    do {
        c = timedRead();
        // compare new character to the next one in target string
        if ((char)c == target[targetIndex])
            targetIndex++;
        else
            targetIndex = 0;
        // if the whole target string was found, return true
        if (targetIndex == targetLength)
            return true;
    } while (c >= 0);
    // if c is -1, it means a timeout occured
    return false;
}
//------------------------------------------------------------------------------
/**
 * Like find() but with a target string residing in program memory.
 *
 * \param[in] target The PROGMEM string to find in the incoming stream.
 *
 * \return true if the target string is found, false if timed out.
 */
bool ExtendedStream::find_P(PGM_P target) {
    uint8_t targetLength = strlen_P(target);
    uint8_t targetIndex = 0;
    int c = 0;
    do {
        c = timedRead();
        // compare new character to the next one in target string
        if ((char)c == pgm_read_byte(target + targetIndex))
            targetIndex++;
        else
            targetIndex = 0;
        // if the whole target string was found, return true
        if (targetIndex == targetLength)
            return true;
    } while (c >= 0);
    // if c is -1, it means a timeout occured
    return false;
}
//------------------------------------------------------------------------------
/**
 * Like findUntil() but with target and terminator strings residing in program
 * memory.
 *
 * \param[in] target The PROGMEM string to find in the incoming stream.
 * \param[in] terminator Upon encountering this PROGMEM string, the search will
 * stop.
 *
 * \return true if the target string is found, false if the terminator string
 * is encountered or a timeout occurs.
 */
bool ExtendedStream::findUntil(char* target, char* terminator) {
    uint8_t targetLength = strlen(target);
    uint8_t termLength = strlen(terminator);
    uint8_t targetIndex = 0;
    uint8_t termIndex = 0;

    // return true if target is a null string
    if (pgm_read_byte(target) == 0)
        return true;

    int c = 0;
    do {
        c = timedRead();

        // compare new character to the next one in target string
        if ((char)c == target[targetIndex])
            targetIndex++;
        else
            targetIndex = 0;
        if (targetIndex == targetLength)
            return true;
        // compare new character to the next one in terminator string
        if ((char)c == terminator[termIndex])
            termIndex++;
        else
            termIndex = 0;
        if (termIndex == termLength)
            return false;
    }  while (c >= 0);
    // if c is -1, it means a timeout occured
    return false;
}
//------------------------------------------------------------------------------
/**
 * Like findUntil() but with target and terminator strings residing in program
 * memory.
 *
 * \param[in] target The PROGMEM string to find in the incoming stream.
 * \param[in] terminator Upon encountering this PROGMEM string, the search will
 * stop.
 *
 * \return true if the target string is found, false if the terminator string
 * is encountered or a timeout occurs.
 */
bool ExtendedStream::findUntil_P(PGM_P target, PGM_P terminator) {
    uint8_t targetLength = strlen_P(target);
    uint8_t termLength = strlen_P(terminator);
    uint8_t targetIndex = 0;
    uint8_t termIndex = 0;

    // return true if target is a null string
    if (pgm_read_byte(target) == 0)
        return true;

    int c = 0;
    do {
        c = timedRead();

        // compare new character to the next one in target string
        if ((char)c == pgm_read_byte(target + targetIndex)) {
            targetIndex++;
        }
        else
            targetIndex = 0;
        if (targetIndex == targetLength)
            return true;
        // compare new character to the next one in terminator string
        if ((char)c == pgm_read_byte(terminator + termIndex))
            termIndex++;
        else
            termIndex = 0;
        if (termIndex == termLength)
            return false;
    }  while (c >= 0);
    // if c is -1, it means a timeout occured
    return false;
}
//------------------------------------------------------------------------------
/**
 * Read characters from serial port into buffer. Terminates if length characters
 * have been read or if a timeout occurs.
 *
 * \param[out] buffer Address of the buffer to write data to.
 * \param[in] length Number of bytes to read into the buffer.
 *
 * \return The number of characters placed in the buffer (0 means no valid data
 * found).
 *
 * \note Unlike Stream::readBytes, this implementation won't stop at null
 * characters and it won't add a null character at the end of the received data,
 * because it has to work with binary data (e.g MP3 files) containing null
 * bytes.
 */
int ExtendedStream::readBytes(char* buffer, size_t length) {
    uint16_t index = 0;
    memset(buffer, 0x00, length);
    while (index < length) {
        int c = timedRead();
        if (c < 0)
            break;
        else
            buffer[index++] = (char)c;
    }
    return index;
}
//------------------------------------------------------------------------------
/**
 * Read characters from serial port into buffer. Terminates if target character
 * is found, or if length characters have been read or if a timeout occurs.
 *
 * \param[in] terminator Upon encountering this character, the search will stop.
 * \param[out] buffer Address of the buffer to write data to.
 * \param[in] length Number of bytes to read into the buffer.
 *
 * \return The number of characters placed in the buffer (0 means no valid data
 * found).
 */
int ExtendedStream::readBytesUntil(char terminator, char* buffer, size_t length) {
    uint16_t index = 0;
    memset(buffer, 0x00, length);
    while (index < length - 1) {
        int c = timedRead();
        if (c < 0)
            break;
        else if ((char)c == terminator)
            break;
        else
            buffer[index++] = (char)c;
    }
    return index;
}
//------------------------------------------------------------------------------
/**
 * Read characters from serial port into buffer. Terminates if terminator string
 * is found, or if length characters have been read or if a timeout occurs.
 *
 * \param[in] terminator Upon encountering this string, the search will stop.
 * \param[out] buffer Address of the buffer to write data to.
 * \param[in] length Number of bytes to read into the buffer.
 *
 * \return The number of characters placed in the buffer (0 means no valid data
 * found).
 */
int ExtendedStream::readBytesUntil(const char* terminator, char* buffer, size_t length) {
    uint8_t termLength = strlen(terminator);
    uint8_t termIndex = 0;
    uint16_t index = 0;
    memset(buffer, 0x00, length);
    while ((index < length - 1) && (termIndex < termLength)) {
        int c = timedRead();
        if (c < 0)
            break;
        buffer[index++] = (char)c;
        if ((char)c == terminator[termIndex])
            termIndex++;
        else
            termIndex = 0;
    }
    return index;
}
//------------------------------------------------------------------------------
/**
 * Like readBytesUntil() but with a terminator string located in the program
 * memory.
 *
 * \param[in] terminator Upon encountering this PROGRAM string, the function
 * will stop.
 * \param[out] buffer Address of the buffer to write data to.
 * \param[in] length Number of bytes to read into the buffer.
 *
 * \return The number of characters placed in the buffer (0 means no valid data
 * found).
 */
int ExtendedStream::readBytesUntil_P(PGM_P terminator, char* buffer, size_t length) {
    uint8_t termLength = strlen_P(terminator);
    uint8_t termIndex = 0;
    uint16_t index = 0;
    memset(buffer, 0x00, length);
    while ((index < length - 1) && (termIndex < termLength)) {
        int c = timedRead();
        if (c < 0)
            break;
        buffer[index++] = (char)c;
        if ((char)c == pgm_read_byte(terminator + termIndex))
            termIndex++;
        else
            termIndex = 0;
    }
    return index;
}
//------------------------------------------------------------------------------
/**
 * This method is redundant with Stream::timedRead() but is still needed for the
 * parsing methods that are re-implemented here, such as readBytes().
 */
int ExtendedStream::timedRead() {
    uint32_t start = millis();
    do {
        int c = read();
        if (c >= 0)
            return c;
    } while (millis() < start + timeout_);
    return -1;
}
//------------------------------------------------------------------------------
/**
 * Send a PROGMEM string to the stream.
 *
 * \param[in] source The Flash-based string to send.
 *
 * \return The number of characters written is returned.
 */
size_t ExtendedStream::write_P(PGM_P source) {
    size_t n = 0;
    do {
        uint8_t ch = pgm_read_byte(source++);
        if (ch == 0x00)
            break;
        else {
            write(ch);
            n++;
        }
    } while (1);
    return n;
}
