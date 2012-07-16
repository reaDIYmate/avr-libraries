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
#include <Settings.h>
//------------------------------------------------------------------------------
/** Name of the API method used to acknowledge the new settings */
const char PROGMEM METHOD_SETTINGS_ACK[] = "app/settings_ack";
/** Name of the API method used to fetch the new settings */
const char PROGMEM METHOD_SETTINGS_GET[] = "app/settings";
/** String returned by the server when the settings are up-to-date */
const char PROGMEM SETTINGS_UP_TO_DATE[] = "null";
/** String returned by the server when the settings are up-to-date */
const char PROGMEM SETTINGS_EMPTY[] = "{}";
//------------------------------------------------------------------------------
/**
 * Construct an instance of Settings.
 *
 * \param[in] settingsNames A Flash-based string array containing the name of
 * each setting.
 * \param[in] nbMaxSettings The number of settings to hold
 * \param[in] api The instance of Api to use for API calls.
 */
Settings::Settings(PGM_P* settingsNames, uint8_t nbMaxSettings,
    Api &api) :
    nbSettings_(0),
    settingsNames_(settingsNames),
    nbMaxSettings_(nbMaxSettings),
    api_(&api),
    settings_(NULL)
{
    // allocate an array to hold the settings
    if (nbMaxSettings_ == 0)
        return;
    settings_ = (char**)malloc(nbMaxSettings_*sizeof(char*));
    if (settings_ != NULL)
        for (uint8_t i = 0; i < nbMaxSettings_; i++)
            settings_[i] = NULL;
}
//------------------------------------------------------------------------------
/** Free the memory allocated for application settings */
Settings::~Settings() {
    if (nbMaxSettings_ > 0 && settings_ != NULL) {
        for (uint8_t i = 0; i < nbMaxSettings_; i++) {
            if (settings_[i] != NULL) {
                free(settings_[i]);
                settings_[i] = NULL;
            }
        }
        free(settings_);
        settings_ = NULL;
    }
}
//------------------------------------------------------------------------------
/**
 * Fetch the application settings from the API and update the local values.
 *
 * \return The integer returned is the number of settings fetched from the API
 * or -1 if the settings need to be restored from the EEPROM.
 */
int Settings::fetch() {
   // if this application never expects any settings, simply return
    if (nbMaxSettings_ == 0)
        return 0;

    int nBytes = api_->call(METHOD_SETTINGS_GET);
    // Api::call returns -1 if the request times out
    if (nBytes <= 0)
        return -1;
    //  the server returns SETTINGS_UP_TO_DATE if everything is up-to-date
    if (api_->findUntil_P(SETTINGS_UP_TO_DATE, PSTR("{")))
        return -1;
    //  the server returns SETTINGS_EMPTY if the new settings are empty
    if (nBytes == 2 && api_->find_P(SETTINGS_EMPTY))
        return 0;

    // parse the new settings
    nbSettings_ = 0;
    for (uint8_t i = 0; i < nbMaxSettings_; i++) {
        // get the name of the next setting
        PGM_P key_P = (char*)pgm_read_word(&settingsNames_[i]);
        // get the corresponding value
        char buffer[64] = {0};
        // detect nested setting names
        const char *dot = strchr_P(key_P, '.');

        // regular setting name
        if (dot == NULL) {
            nBytes = api_->getStringByName_P(key_P, buffer, 64);
        }
        // nested setting name
        else {
            // copy key to SRAM
            char key[17] = {0};
            strcpy_P(key, key_P);

            // key format is "service.property"
            char service[11] = {0};
            char property[7] = {0};
            sscanf(key, "%[^.].%s", service, property);

            // parse service
            char objectString[64] = {0};
            nBytes = api_->getObjectStringByName(service, objectString, 64);
            if (nBytes < 0)
                continue;

            // parse property
            JsonStream json(objectString);
            nBytes = json.getStringByName(property, buffer, 64);
        }

        if (nBytes <= 0)
            continue;

        // allocate memory for the setting
        char* newSetting = (char*)calloc(strlen(buffer) + 1,sizeof(char));

        // copy the setting value
        strcpy(newSetting, buffer);
        settings_[i] = newSetting;
        ++nbSettings_;
    }

    // update configuration status
    if (api_->call(METHOD_SETTINGS_ACK) < 0)
        return -1;
    // didn't find SETTINGS_EMPTY but then failed to parse anything
    if (nbSettings_ == 0)
        return -1;

    return nbSettings_;
}
//------------------------------------------------------------------------------
/**
 * Restore the settings from the EEPROM to the SRAM.
 *
 * \return The integer returned is the number of settings restored to SRAM.
 *
 * \note The size will be 0x000 if the setting was empty the last time it was
 * fetched. It might also be 0xFFFF if the previous number of settings in the
 * app was less than the current one, since 0xFFFF is the default value of the
 * EEPROM for addresses that have never been written to.
 */
int Settings::restore() {
    uint16_t addr = EEPROM_SETTINGS;

    nbSettings_ = 0;
    for (uint8_t i = 0; i < nbMaxSettings_; i++) {
        // read the setting size
        uint8_t lowByte = eeprom_read_byte((uint8_t*)addr);
        uint8_t highByte = eeprom_read_byte((uint8_t*)(addr + 1));
        addr += 2;
        uint16_t size = ((highByte << 8) & 0xFF00) + (lowByte & 0xFF);

        // skip empty settings
        if (size == 0x0000 || size == 0xFFFF)
           continue;

        // allocate memory for the setting
        char* newSetting = (char*)malloc((size + 1)*sizeof(char));

        // read the setting value
        for (uint16_t j = 0; j < size; j++) {
            char c = eeprom_read_byte((uint8_t*)addr);
            newSetting[j] = c;
            addr++;
        }

        newSetting[size] = '\0';
        settings_[i] = newSetting;
        ++nbSettings_;
    }
    return nbSettings_;
}
//------------------------------------------------------------------------------
/** Save the settings from the SRAM to the EEPROM */
void Settings::save() {
    uint16_t addr = EEPROM_SETTINGS;

    for (uint8_t i = 0; i < nbMaxSettings_; i++) {
        uint16_t size = 0;

        // check for null pointers
        if (settings_[i] != NULL) {
            size = strlen(settings_[i]);
        }

        // write the settings size
        uint8_t lowByte = size & 0xFF;
        uint8_t highByte = (size >> 8) & 0xFF;
        eeprom_write_byte((uint8_t*)addr, lowByte);
        eeprom_write_byte((uint8_t*)(addr + 1), highByte);
        addr += 2;

        // write the settings value
        for (uint16_t j = 0; j < size; j++) {
            eeprom_write_byte((uint8_t*)addr, settings_[i][j]);
            addr++;
        }
    }
}
