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
const char SD_FILE[] PROGMEM = "TWEET.TXT";
//------------------------------------------------------------------------------
Twitter::Twitter(Api &api, Settings &settings, PGM_P motion, PGM_P sound,
        PGM_P alertEnabled, PGM_P actionEnabled, SdFat &sd,
        uint8_t sdChipSelectPin) :
    Service(api, settings, alertEnabled, motion, sound),
    ContentAction(api, settings, actionEnabled, sd, sdChipSelectPin, SD_FILE)
{
}
//------------------------------------------------------------------------------
int Twitter::fetch() {
    Api* api = Service::api_;
    api->call(STRING_API_TWITTER_UPDATE);
    int mentions = api->getIntegerByName_P(KEY_MENTIONS);
    int followers = api->getIntegerByName_P(KEY_FOLLOWERS);

    return mentions + followers;
}
//------------------------------------------------------------------------------
bool Twitter::perform() {
    char content[140] = {0};
    read(content, 140);
    int nBytes = Action::api_->call(STRING_API_TWITTER_POST, KEY_STATUS, content);
    return (nBytes > 0);
}
//------------------------------------------------------------------------------
bool Twitter::updateContent() {
    char content[140] = {0};

    Api* api = Action::api_;
    api->call(STRING_API_TWITTER_AUTO);
    api->rewind();
    if (api->find_P(SETTINGS_UP_TO_DATE)) {
        return false;
    }
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    char filename[13] = {0};
    strcpy_P(filename, filename_);
    if (sd_->exists(filename)){
        sd_->remove(filename);
    }
    if (!open(filename, O_CREAT | O_WRITE)) {
        return false;
    }
    int nBytes = api->getStringByName_P(KEY_MSG, content, 140);
    if (nBytes > -1) {
        write(content, nBytes);
        sync();
    }
    close();
    return (nBytes > -1);
}
