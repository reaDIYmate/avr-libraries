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
const char SD_FILE[] = "FBSTATUS.TXT";
//------------------------------------------------------------------------------
Facebook::Facebook(Api &api, Settings &settings, SdFat &sd, uint8_t sdChipSelectPin,
    PGM_P motion, PGM_P sound, PGM_P actionEnabled, PGM_P alertEnabled) :
    Service(api, settings, alertEnabled, motion, sound),
    Action(api, settings, actionEnabled),
    sd_(&sd),
    sdChipSelectPin_(sdChipSelectPin)
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
bool Facebook::postStatus() {
     if (!Action::enabled())
        return false;

    char content[140] = {0};
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    if (!open(SD_FILE, O_READ)) {
        return false;
    }
    rewind();
    read(content, 140);

    Api* api = Action::api_;
    api->call(STRING_API_FACEBOOK_POST, KEY_STATUS, content);

    int status = api->getIntegerByName_P(KEY_STATUS);
    close();
    return (status == 0);
}
//------------------------------------------------------------------------------
bool Facebook::saveSettings() {
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
        write(status, nbBytes);
        sync();
    }
    close();
    return true;
}
