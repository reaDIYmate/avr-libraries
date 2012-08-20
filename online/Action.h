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
#ifndef ACTION_H
#define ACTION_H

#include <Api.h>
#include <Settings.h>
//------------------------------------------------------------------------------
class Action {
public:
    Action(Api &api, Settings &settings, PGM_P enabled);
    bool enabled();
//------------------------------------------------------------------------------
protected:
    Api *api_;
    Settings *settings_;
//------------------------------------------------------------------------------
private:
    PGM_P enabled_;
};

#endif // ACTION_H;
