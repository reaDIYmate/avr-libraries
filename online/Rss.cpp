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
#include "Rss.h"
//------------------------------------------------------------------------------
const char STRING_API_RSS[] PROGMEM = "rss/update";
const char KEY_KEYWORD[] PROGMEM = "keyword";
const char KEY_FEED_URL[] PROGMEM = "feed_url";
//------------------------------------------------------------------------------
Rss::Rss(Api &api, Settings &settings, PGM_P on, PGM_P motionName,
    PGM_P soundName, PGM_P keyword, PGM_P feed_url) :
    api_(&api),
    settings_(&settings),
    count_(-1),
    on_(on),
    motionName_(motionName),
    soundName_(soundName),
    keyword_(keyword),
    feed_url_(feed_url)
{
}
//------------------------------------------------------------------------------
bool Rss::update(){
    if (strcmp("1",settings_->getByName(on_)) == 0) {
        int countCurrent;
        api_->call(STRING_API_RSS, KEY_KEYWORD, settings_->getByName(keyword_),
            KEY_FEED_URL, settings_->getByName(feed_url_));
        countCurrent = api_->getIntegerByName("count");

        if (countCurrent != count_ && count_ != -1) {
            if (countCurrent > count_ ) {
                count_ = countCurrent;
                return true;
            }
            else {
                count_ = countCurrent;
            }
        }
        if (count_ == -1) {
            count_ = countCurrent;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
char* Rss::getMotionFilename() {
    return settings_->getByName(motionName_);
}
//------------------------------------------------------------------------------
char* Rss::getSoundFilename() {
    return settings_->getByName(soundName_);
}
