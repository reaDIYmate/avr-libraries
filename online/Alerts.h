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
#ifndef ALERTS_H
#define ALERTS_H
/**
 * \file
 * \brief Alerts class to fetch alerts from the reaDIYmate webservice.
 */
#include <avr/pgmspace.h>
#include <Api.h>
#include <Settings.h>
//------------------------------------------------------------------------------
/**
 * \class Alerts
 * \brief Poll the reaDIYmate webservice for new alerts.
 */
class Alerts {
public:
    Alerts(Api &api, Settings& settings);
    uint8_t fetch();
    bool isActive(uint8_t i);
    void synchronize();
    char* getFilename();
//------------------------------------------------------------------------------
private:
    int readNewValue(uint8_t i);
    bool repeatEnabled(uint8_t i);
    /** Cache the answer of the API */
    int matchValues_[8];
    /** Cache the index of the last alert accessed */
    uint8_t index_;
    /** Instance of Api used to access the reaDIYmate API */
    Api* api_;
    /** Instance of Settings used to retrieve the application settings */
    Settings* settings_;
};

#endif // ALERTS_H
