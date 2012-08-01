/* reaDIYmate AVR library
 * Written by Christopher Schindler
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
#include "Twitter.h"
//------------------------------------------------------------------------------
const char STRING_API_TWITTER_UPDATE[] PROGMEM = "twitter/update";
const char STRING_API_TWITTER_POST[] PROGMEM = "twitter/post_status";
const char STRING_API_TWITTER_AUTO[] PROGMEM = "twitter/auto";
const char KEY_FOLLOWERS[] PROGMEM = "followers";
const char KEY_MENTIONS[] PROGMEM = "mentions";
const char KEY_STATUS[] PROGMEM = "status";
const char KEY_MSG[] PROGMEM = "msg";
const char SETTINGS_UP_TO_DATE[] PROGMEM = "null";
const char SD_FILE[] = "TWEET.TXT";
//------------------------------------------------------------------------------
Twitter::Twitter(Api &api, SdFat &sd, Settings &settings, PGM_P on, PGM_P motion,
    PGM_P sound, PGM_P action, uint8_t sdChipSelectPin) :
    Service(api, settings, on, motion, sound),
    action_(action),
    sd_(&sd),
    sdChipSelectPin_(sdChipSelectPin)
{
}
//------------------------------------------------------------------------------
int Twitter::fetch() {
    api_->call(STRING_API_TWITTER_UPDATE);
    int mentions = api_->getIntegerByName_P(KEY_MENTIONS);
    int followers = api_->getIntegerByName_P(KEY_FOLLOWERS);

    return mentions + followers;
}
//------------------------------------------------------------------------------
bool Twitter::postStatus() {
    char status[140] = {0};
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    if (!open(SD_FILE, O_READ)) {
        Serial.println(F("Failed opening"));
        return false;
    }
    rewind();
    read(status, 140);
    if (strcmp("1", settings_->getByName(action_)) == 0) {
        api_->call(STRING_API_TWITTER_POST, KEY_STATUS, status);
        if (api_->getIntegerByName_P(KEY_STATUS) == 0){
            close();
            return true;
        }
    }
    close();
    return false;
}
//------------------------------------------------------------------------------
bool Twitter::saveSettings() {
    char status[140] = {0};

    api_->call(STRING_API_TWITTER_AUTO);
    api_->rewind();
    if(api_->find_P(SETTINGS_UP_TO_DATE)){
        return false;
    }
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    if (sd_->exists(SD_FILE)){
        sd_->remove(SD_FILE);
    }
    if (!open(SD_FILE, O_CREAT | O_WRITE)) {
        return false;
    }
    int nbBytes = api_->getStringByName_P(KEY_MSG, status, 140);
    if(nbBytes > -1){
        write(status, nbBytes);
        sync();
    }
    close();
    return true;
}
