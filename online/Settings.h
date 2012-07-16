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
#ifndef SETTINGS_H
#define SETTINGS_H
/**
 * \file
 * \brief Settings class to manage application settings in the EEPROM.
 */
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <Api.h>
#include <eepromAddresses.h>
//------------------------------------------------------------------------------
/**
 * \class Settings
 * \brief Read, write and update the settings stored in the EEPROM.
 */
class Settings {
public:
    Settings(PGM_P* settingsNames, uint8_t nbMaxSettings, Api &api);
    ~Settings();
    int fetch();
    int restore();
    void save();
    /** Return a pointer to a given setting */
    char* getSettingByNumber(uint8_t n) {return settings_[n];}
    /** Return the number of settings */
    uint8_t getNbSettings() {return nbSettings_;}
//------------------------------------------------------------------------------
private:
    /** The maximum number of elements in the settings */
    const uint8_t nbMaxSettings_;
    /** The number of elements in the settings */
    uint8_t nbSettings_;
    /** The individual names of the settings, stored in Flash */
    PGM_P* settingsNames_;
    /** An array of settings */
    char** settings_;
    /** Instance of Api used to access the reaDIYmate API */
    Api* api_;
};

#endif // SETTINGS_H
