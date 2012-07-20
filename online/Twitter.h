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
#ifndef TWITTER_H
#define TWITTER_H

#include <Api.h>
#include <Settings.h>
//------------------------------------------------------------------------------
class Twitter {
public:
    Twitter(Api &api, Settings &settings, PGM_P on, PGM_P motionName,
        PGM_P soundName);
    bool update();
    char* getMotionFilename();
    char* getSoundFilename();
//------------------------------------------------------------------------------
private:
    Api *api_;
    Settings *settings_;
    int count_;
    PGM_P on_;
    PGM_P motionName_;
    PGM_P soundName_;
};

#endif // TWITTER_H
