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
#ifndef SERIAL_STREAM_H
#define SERIAL_STREAM_H
/**
 * \file
 * \brief SerialStream class which combines functionnality from ExtendedStream
 * and the HardwareSerial Arduino core.
 */
#include <Arduino.h>
#include <ExtendedStream.h>
#include <HardwareSerial.h>
//------------------------------------------------------------------------------
/**
 * \class SerialStream
 * \brief Encapsulates printing and parsing functions for serial data.
 */
class SerialStream : public ExtendedStream {
public:
    SerialStream(HardwareSerial &serial, uint32_t timeout);
    ~SerialStream();
    // pure virtual functions similar to those of the Stream interface
    /**
     * Check the number of available characters.
     *
     * \return The number of available characters is returned.
     */
    virtual int available() {return serial_.available();}
    /** Make sure all outgoing data is sent */
    virtual void flush() {return serial_.flush();}
    /**
     * Read one character from the stream.
     *
     * \return If a valid character is read its value is returned, otherwise the
     * value -1 is returned.
     */
    virtual int read() {return serial_.read();}
    /** Write one byte to the stream */
    virtual size_t write(uint8_t c) {return serial_.write(c);}
    /** Open the stream */
    void begin(uint32_t baud) {serial_.begin(baud);}
//------------------------------------------------------------------------------
private:
    /** Preinstantiated HardwareSerial object */
    HardwareSerial serial_;
};

#endif // SERIAL_STREAM_H
