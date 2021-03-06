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
#include "ContentAction.h"
//------------------------------------------------------------------------------
ContentAction::ContentAction(Api &api, Settings &settings, PGM_P enabled,
    SdFat &sd, uint8_t sdChipSelectPin, PGM_P filename) :
    Action(api, settings, enabled),
    sd_(&sd),
    sdChipSelectPin_(sdChipSelectPin),
    filename_(filename)
{
}
//------------------------------------------------------------------------------
bool ContentAction::trigger() {
    if (!enabled())
        return false;
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    char filename[13] = {0};
    strcpy_P(filename, filename_);
    if (!open(filename, O_READ)) {
        return false;
    }
    rewind();
    bool success = Action::trigger();
    close();
    return success;
}
