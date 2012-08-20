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
#ifndef FACEBOOK_H
#define FACEBOOK_H

#include <Action.h>
#include <SdFat.h>
#include <Service.h>
//------------------------------------------------------------------------------
class Facebook : public Service, public Action, public SdFile {
public:
    Facebook(Api &api, Settings &settings, SdFat &sd, uint8_t sdChipSelectPin,
        PGM_P motion, PGM_P sound, PGM_P actionEnabled, PGM_P alertEnabled);
    int fetch();
    bool postStatus();
    bool saveSettings();
//------------------------------------------------------------------------------
private:
    SdFat* sd_;
    const uint8_t sdChipSelectPin_;
};

#endif // FACEBOOK_H
