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
const char KEY_COUNT[] PROGMEM = "count";
const char KEY_KEYWORD[] PROGMEM = "keyword";
//------------------------------------------------------------------------------
Gmail::Gmail(Api &api, Settings &settings, PGM_P on, PGM_P motion,
    PGM_P sound, PGM_P keyword) :
    Service(api, settings, on, motion, sound),
    keyword_(keyword)
{
}
//------------------------------------------------------------------------------
int Gmail::fetch() {
    api_->call(STRING_API_GMAIL, KEY_KEYWORD, settings_->getByName(keyword_));
    return api_->getIntegerByName_P(KEY_COUNT);
}
