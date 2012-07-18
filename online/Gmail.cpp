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
#include "Gmail.h"
//------------------------------------------------------------------------------
const char STRING_API_GMAIL[] PROGMEM = "gmail/search";
//------------------------------------------------------------------------------
Gmail::Gmail(Api &api, Settings &settings, PGM_P motionName, PGM_P soundName) :
    api_(&api),
    settings_(&settings),
    count_(-1),
    motionName_(motionName),
    soundName_(soundName)
{
}
//------------------------------------------------------------------------------
bool Gmail::update() {
    int countCurrent;

    api_->call(STRING_API_GMAIL);
    countCurrent = api_->getIntegerByName("count");

    if (countCurrent != count_ && count_ != -1) {
        if (countCurrent > count_) {
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
    return false;
}
//------------------------------------------------------------------------------
char* Gmail::getMotionFilename() {
    return settings_->getByName(motionName_);
}
//------------------------------------------------------------------------------
char* Gmail::getSoundFilename() {
    return settings_->getByName(soundName_);
}
