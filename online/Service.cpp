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
 #include "Service.h"
//------------------------------------------------------------------------------
Service::Service(Api &api, Settings &settings, PGM_P enabled, PGM_P motion,
    PGM_P sound) :
    api_(&api),
    settings_(&settings),
    count_(-1),
    enabled_(enabled),
    motion_(motion),
    sound_(sound)
{
}
//------------------------------------------------------------------------------
bool Service::enabled() {
   return (strcmp("1", settings_->getByName(enabled_)) == 0);
}
//------------------------------------------------------------------------------
bool Service::update() {
    if (!enabled())
        return false;

    int newCount = fetch();
    if ((newCount != count_) && (count_ != -1) && (newCount != 0)) {
        if (newCount > count_ ) {
            count_ = newCount;
            return true;
        }
        else {
            count_ = newCount;
        }
    }
    if (count_ == -1) {
        count_ = newCount;
    }
    return false;
 }
//------------------------------------------------------------------------------
char* Service::getMotionFilename() {
    return settings_->getByName(motion_);
}
//------------------------------------------------------------------------------
char* Service::getSoundFilename() {
    return settings_->getByName(sound_);
}
