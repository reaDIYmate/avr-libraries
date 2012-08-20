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
 #include "Facebook.h"
//------------------------------------------------------------------------------
const char STRING_API_FACEBOOK_UPDATE[] PROGMEM = "facebook/update";
const char STRING_API_FACEBOOK_POST[] PROGMEM = "facebook/post_status";
const char STRING_API_FACEBOOK_AUTO[] PROGMEM = "facebook/auto";
const char KEY_FRIEND_REQUESTS[] PROGMEM = "friendrequests";
const char KEY_NOTIFICATIONS[] PROGMEM = "notifications";
const char KEY_POKES[] PROGMEM = "pokes";
const char KEY_STATUS[] PROGMEM = "status";
const char KEY_MSG[] PROGMEM = "msg";
const char SETTINGS_UP_TO_DATE[] PROGMEM = "null";
const char SD_FILE[] PROGMEM = "FBSTATUS.TXT";
//------------------------------------------------------------------------------
    Facebook::Facebook(Api &api, Settings &settings, PGM_P motion, PGM_P sound,
         PGM_P alertEnabled, PGM_P actionEnabled, SdFat &sd,
         uint8_t sdChipSelectPin) :
    Service(api, settings, alertEnabled, motion, sound),
    ContentAction(api, settings, actionEnabled, sd, sdChipSelectPin, SD_FILE)
{
}
//------------------------------------------------------------------------------
int Facebook::fetch() {
    Api* api = Service::api_;
    api->call(STRING_API_FACEBOOK_UPDATE);
    int pokes = api->getIntegerByName_P(KEY_POKES);
    int notifications = api->getIntegerByName_P(KEY_NOTIFICATIONS);
    int friendRequests = api->getIntegerByName_P(KEY_FRIEND_REQUESTS);

    return pokes + notifications + friendRequests;
}
//------------------------------------------------------------------------------
bool Facebook::perform() {
    char content[140] = {0};
    read(content, 140);
    int nBytes = Action::api_->call(STRING_API_FACEBOOK_POST, KEY_STATUS, content);
    return (nBytes > 0);
}
//------------------------------------------------------------------------------
bool Facebook::updateContent() {
    char content[140] = {0};

    Api* api = Action::api_;
    api->call(STRING_API_FACEBOOK_AUTO);
    api->rewind();
    if (api->find_P(SETTINGS_UP_TO_DATE)){
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
    int nbBytes = api->getStringByName_P(KEY_MSG, content, 140);
    if (nbBytes > -1) {
        write(content, nbBytes);
        sync();
    }
    close();
    return true;
}
