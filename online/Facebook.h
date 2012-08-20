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

#include <ContentAction.h>
#include <Service.h>
//------------------------------------------------------------------------------
class Facebook : public Service, public ContentAction {
public:
    Facebook(Api &api, Settings &settings, PGM_P motion, PGM_P sound,
         PGM_P alertEnabled, PGM_P actionEnabled, SdFat &sd,
         uint8_t sdChipSelectPin);
    int fetch();
    bool perform();
    bool updateContent();
//------------------------------------------------------------------------------
};

#endif // FACEBOOK_H
