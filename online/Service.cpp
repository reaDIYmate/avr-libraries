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
Service::Service(Api &api, Settings &settings, PGM_P on, PGM_P motion,
    PGM_P sound) :
    api_(&api),
    settings_(&settings),
    count_(-1),
    on_(on),
    motion_(motion),
    sound_(sound)
{
}
//------------------------------------------------------------------------------
bool Service::update() {
   if (strcmp("1", settings_->getByName(on_)) == 0) {
        int countCurrent = fetch();
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
char* Service::getMotionFilename() {
    return settings_->getByName(motion_);
}
//------------------------------------------------------------------------------
char* Service::getSoundFilename() {
    return settings_->getByName(sound_);
}
