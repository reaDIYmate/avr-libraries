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
#ifndef RESOURCES_H
#define RESOURCES_H
/**
 * \file
 * \brief Resources class to manage resource files on the object.
 */
#include <Api.h>
#include <Download.h>
//------------------------------------------------------------------------------
/**
 * \class Resources
 * \brief Synchronize files with the list maintained by the reaDIYmate server.
 */
class Resources : public Download {
public:
    Resources(Api &api, Wifly &wifly, char* buffer, size_t bufferSize, PGM_P host,
        SdFat &sd, uint8_t sdChipSelectPin);
    bool synchronize();
//------------------------------------------------------------------------------
private:
    /** Instance of Api used to access the reaDIYmate API */
    Api* api_;
};

#endif // RESOURCES_H
