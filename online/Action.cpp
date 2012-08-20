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
#include "Action.h"
//------------------------------------------------------------------------------
const char KEY_STATUS[] PROGMEM = "status";
//------------------------------------------------------------------------------
Action::Action(Api &api, Settings &settings, PGM_P enabled) :
    api_(&api),
    settings_(&settings),
    enabled_(enabled)
{
}
//------------------------------------------------------------------------------
bool Action::enabled() {
    return (strcmp("1", settings_->getByName(enabled_)) == 0);
}
//------------------------------------------------------------------------------
bool Action::trigger() {
    if (!perform())
        return false;
    int status = api_->getIntegerByName_P(KEY_STATUS);
    return (status == 0);
}
