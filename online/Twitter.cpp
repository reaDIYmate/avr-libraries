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
const char STRING_API_TWITTER[] PROGMEM = "twitter/update";
//------------------------------------------------------------------------------
Twitter::Twitter(Api &api, Settings &settings, PGM_P on, PGM_P motion,
    PGM_P sound) :
    Service(api, settings, on, motion, sound)
{
}
//------------------------------------------------------------------------------
int Twitter::fetch() {
    api_->call(STRING_API_TWITTER);
    int mentions = api_->getIntegerByName("mentions");
    int followers = api_->getIntegerByName("followers");
    return mentions + followers;
}
