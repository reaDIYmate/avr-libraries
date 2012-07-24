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
const char KEY_FOLLOWERS[] PROGMEM = "followers";
const char KEY_MENTIONS[] PROGMEM = "mentions";
const char KEY_STATUS[] PROGMEM = "status";
//------------------------------------------------------------------------------
Twitter::Twitter(Api &api, Settings &settings, PGM_P on, PGM_P motion,
    PGM_P sound, PGM_P action) :
    Service(api, settings, on, motion, sound),
    action_(action)
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
    //WORK IN PROGRESS
    if (strcmp("1", settings_->getByName(action_)) == 0) {
        api_->call(STRING_API_TWITTER_POST, KEY_STATUS, "reaDIYmate");
        
        char buffer[4];
        api_->getStringByName_P(KEY_STATUS, buffer, 4);
        if (strcmp("OK", buffer) == 0){
            return true;
        }
    }
    return false;
}
