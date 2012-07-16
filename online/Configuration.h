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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H
/**
 * \file
 * \brief Configuration class to store and retrieve settings from the EEPROM.
 */
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <eepromAddresses.h>
#include <JsonStream.h>
#include <SerialStream.h>
#include <Wifly.h>
#include <StatusLed.h>
//------------------------------------------------------------------------------
/**
 * \class Configuration
 * \brief Read, write and update the object configuration stored in the EEPROM.
 */
class Configuration : public SerialStream {
public:
    Configuration(HardwareSerial &companion, Wifly &wifly, StatusLed &led);
    ~Configuration();
    void getApiCredential(char* buffer, uint8_t bufferSize);
    const char* getPusherKey() {return key_;}
    const char* getPusherSecret() {return secret_;}
    const char* getPusherChannel() {return channel_;}
    void synchronize(uint16_t timeout);
//------------------------------------------------------------------------------
private:
    void readUserAndPass(char* buffer, uint8_t bufferSize);
    void readWifiSettings(char* buffer, uint8_t bufferSize);
    void restoreDeviceId();
    void restorePusher();
    void restoreUserAndPass();
    void saveDeviceId();
    void savePusher();
    void saveUserAndPass();
    void sendDeviceId();
    /** The unique identifier of the device */
    char deviceId_[13];
    /** The username for the reaDIYmate webservices */
    char* username_;
    /** The password for the reaDIYmate webservices */
    char* password_;
    /** Public key for the Pusher application */
    char* key_;
    /** Private key for the Pusher application */
    char* secret_;
    /** Active channel on the Pusher application */
    char* channel_;
    /** The WiFly module manager */
    Wifly* wifly_;
    /** The dual-color LED wrapper */
    StatusLed* led_;
};

#endif // CONFIGURATION_H
