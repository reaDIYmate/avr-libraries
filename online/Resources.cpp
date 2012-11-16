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
#include <Resources.h>
//------------------------------------------------------------------------------
// reaDIYmate API strings
/** Name of the API method used to enquire about resource status */
const char PROGMEM METHOD_RESOURCE_SYNC[] = "resources/get_one";
/** Name of the API method used to confirm resource synchronization */
const char PROGMEM METHOD_RESOURCE_ACK[] = "resources/ack_one";
/** Key corresponding to the resource ID in the API response */
const char PROGMEM KEY_RESOURCE_ID_GET[] = "id";
/** Key corresponding to the resource ID in the API call */
const char PROGMEM KEY_RESOURCE_ID_ACK[] = "resource";
/** Key corresponding to the resource name in the API response */
const char PROGMEM KEY_RESOURCE_NAME[] = "short_name";
/** Key corresponding to the resource URL in the API response */
const char PROGMEM KEY_RESOURCE_URL[] = "url";
/** Key corresponding to the resource status flag in the API response */
const char PROGMEM KEY_RESOURCE_FLAG[] = "delete_pending";
/** Content of an empty response */
const char PROGMEM EMPTY_RESPONSE[] = "[]";
/** Value of the status flag meaning the resource should be deleted */
uint8_t const FLAG_DELETE = 0x01;
/** Value of the status flag meaning the resource should be updated */
uint8_t const FLAG_UPDATE = 0x00;
//------------------------------------------------------------------------------
/**
 * Construct an instance of Resources.
 *
 * \param[in] api The instance of Api to use for API calls.
 * \param[in] wifly The Wifly object to use for communications.
 * \param[in] buffer The Buffer to use for downloading.
 * \param[in] bufferSize The size of the buffer.
 * \param[in] host The remote host where the API is located.
 * \param[in] sd A SdFat object.
 * \param[in] sdChipSelectPin The AVR pin connected to CS on the SD card.
 */
Resources::Resources(Api &api, Wifly &wifly, char* buffer,
    size_t bufferSize, PGM_P host, SdFat &sd, uint8_t sdChipSelectPin) :
    Download(wifly, buffer, bufferSize, host, sd, sdChipSelectPin),
    api_(&api)
{
}
//------------------------------------------------------------------------------
/**
 * Synchronize the resources on the SD card with the list maintained by the
 * reaDIYmate server.
 *
 * \return true if the synchronization runs to completion, false if it fails.
 */
bool Resources::synchronize() {
    do {
        // request a resource to synchronize
        if (api_->call(METHOD_RESOURCE_SYNC) < 0) {
#ifdef DEBUG
            Serial.println(F("API call failed"));
#endif
            return false;
        }
#ifdef DEBUG
        Serial.print(F("API response: "));
        Serial.println(buffer_);
#endif

        // check for valid empty responses
        if (strcmp_P(buffer_, EMPTY_RESPONSE) == 0)
            return true;

        // extract resource information
        int id = api_->getIntegerByName_P(KEY_RESOURCE_ID_GET);
        if (id < 0)
            return false;

        uint8_t flag = api_->getIntegerByName_P(KEY_RESOURCE_FLAG);

        char name[32] = {0};
        api_->getStringByName_P(KEY_RESOURCE_NAME, name, 32);
#ifdef DEBUG
        Serial.print(F("Downloading "));
        Serial.println(name);
#endif

        char url[64] = {0};
        api_->getStringByName_P(KEY_RESOURCE_URL, url, 64);

        // synchronize resource
        if (exists(name))
            remove(this, name);
        switch (flag) {
            case FLAG_UPDATE :
                strlcat(url, name, 64);
                if (!save(url, name)) {
#ifdef DEBUG
                    Serial.println(F("Download failed."));
#endif
                    return false;
                }
                break;
            case FLAG_DELETE :
                break;
            default :
                return false;
        }
#ifdef DEBUG
        Serial.println(F("Download complete."));
#endif
        // confirm resource synchronization
        char idBuffer[8] = {0};
        snprintf(idBuffer, 8, "%d", id);
        if (api_->call(METHOD_RESOURCE_ACK, KEY_RESOURCE_ID_ACK, idBuffer) < 0)
            return false;
    } while (1);
}
