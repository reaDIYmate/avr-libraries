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
#include "Foursquare.h"
//------------------------------------------------------------------------------
const char STRING_API_FOURSQUARE_UPDATE[] PROGMEM = "foursquare/update";
const char STRING_API_FOURSQUARE_CHECKIN[] PROGMEM = "foursquare/checkin";
const char KEY_CHECKINS[] PROGMEM = "checkins";
const char KEY_VENUEID[] PROGMEM = "venueId";
//------------------------------------------------------------------------------
Foursquare::Foursquare(Api &api, Settings &settings, PGM_P motion, PGM_P sound,
    PGM_P checkId, PGM_P venueId, PGM_P actionEnabled, PGM_P alertEnabled) :
    Service(api, settings, alertEnabled, motion, sound),
    Action(api, settings, actionEnabled),
    checkId_(checkId),
    venueId_(venueId)
{
}
//------------------------------------------------------------------------------
int Foursquare::fetch() {
    Api* api = Service::api_;
    api->call(STRING_API_FOURSQUARE_UPDATE, KEY_VENUEID,
        Service::settings_->getByName(venueId_));
    return api->getIntegerByName_P(KEY_CHECKINS);
}
//------------------------------------------------------------------------------
bool Foursquare::perform() {
    int nBytes = Action::api_->call(STRING_API_FOURSQUARE_CHECKIN, KEY_VENUEID,
        Action::settings_->getByName(checkId_));
    return (nBytes > 0);
}
//------------------------------------------------------------------------------
