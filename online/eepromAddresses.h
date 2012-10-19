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
#ifndef EEPROM_ADDRESSES_H
#define EEPROM_ADDRESSES_H
//------------------------------------------------------------------------------
// EEPROM addresses
/** 12 bytes for the device ID */
uint16_t const EEPROM_DEVICEID = 0x00;
/** 32 bytes for the user name */
uint16_t const EEPROM_USERNAME = 0x0C;
/** 64 bytes for the password hash */
uint16_t const EEPROM_PASSWORD = 0x2C;
/** 21 bytes for the Pusher application key */
uint16_t const EEPROM_PUSHER_KEY = 0x6C;
/** 21 bytes for the Pusher application secret */
uint16_t const EEPROM_PUSHER_SECRET = 0x81;
/** 21 bytes for the Pusher application channel */
uint16_t const EEPROM_PUSHER_CHANNEL = 0x96;
/** 1 byte for the default position of the servo */
uint16_t const EEPROM_SERVO_ORIGIN = 0xAB;
/** Application settings are stored in the remaining EEPROM space */
uint16_t const EEPROM_SETTINGS = 0xAC;

#endif // EEPROM_ADDRESSES_H
