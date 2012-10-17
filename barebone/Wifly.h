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
#ifndef WIFLY_H
#define WIFLY_H
/**
 * \file
 * \brief Wifly class for the Roving Networks GSX/EZX Wi-Fi modules.
 */
#include <avr/pgmspace.h>
#include <digitalWriteFast.h>
#include <SerialStream.h>
#include <BufferedStream.h>
//------------------------------------------------------------------------------
/** Size of the buffer used to hold the host name */
uint8_t const WIFLY_HOST_BUFFER_SIZE = 32;
//------------------------------------------------------------------------------
/**
 * \class Wifly
 * \brief Send HTTP requests and fetch data with a RN131 or RN171 module.
 */
class Wifly : public SerialStream {
public:
    Wifly(HardwareSerial &serial, uint8_t resetPin, uint8_t gpio4Pin,
        uint8_t gpio5Pin, uint8_t gpio6Pin);
    bool awaitResponse();
    bool connect(const char* host);
    bool connectedTo_P(PGM_P host);
    void disconnect();
    void getDeviceId(char* output);
    void initialize();
    void reset();
    bool resetBaudrate();
    bool resetConfigToDefault();
    bool setWlanConfig(const char* ssid, const char* passphrase,
        const char* ip = NULL, const char* mask = NULL,
        const char* gateway = NULL);
    bool updateFirmware();
//------------------------------------------------------------------------------
private:
    bool associated();
    bool associated(uint16_t timeout);
    void closeSocket();
    bool connected();
    bool connected(uint16_t timeout);
    bool enterCommandMode();
    bool executeCommand(PGM_P commandIndex, PGM_P expectedReturnIndex,
        const char* parameter = NULL);
    bool executeCommand(PGM_P commandIndex, PGM_P expectedReturnIndex,
        long parameter);
    void join();
    void openSocket();
    bool setHost(const char* host);
    /** Hardware reset pin */
    const uint8_t resetPin_;
    /** Access point status pin (GPIO4) */
    const uint8_t gpio4Pin_;
    /** TCP connection control pin (GPIO5) */
    const uint8_t gpio5Pin_;
    /** TCP connection status pin (GPIO6) */
    const uint8_t gpio6Pin_;
    /** Host name buffer */
    char host_[WIFLY_HOST_BUFFER_SIZE];
};

#endif // WIFLY_H
