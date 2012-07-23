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
const char KEY_COUNT[] PROGMEM = "count";
const char KEY_FEED_URL[] PROGMEM = "feed_url";
const char KEY_KEYWORD[] PROGMEM = "keyword";
//------------------------------------------------------------------------------
Rss::Rss(Api &api, Settings &settings, PGM_P on, PGM_P motion,
    PGM_P sound, PGM_P keyword, PGM_P feedUrl) :
    Service(api, settings, on, motion, sound),
    keyword_(keyword),
    feedUrl_(feedUrl)
{
}
//------------------------------------------------------------------------------
int Rss::fetch(){
    api_->call(STRING_API_RSS, KEY_KEYWORD, settings_->getByName(keyword_),
        KEY_FEED_URL, settings_->getByName(feedUrl_));
    return api_->getIntegerByName_P(KEY_COUNT);
}
