/* reaDIYmate AVR library
 * Written by Pierre Bouchet
 * Copyright (C) 2011-2012 reaDIYmate
 *
 * This file is part of the reaDIYmate library
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
#ifndef DISPATCHER_H
#define DISPATCHER_H
/**
 * \file
 * \brief -
 */
#include <Api.h>
#include <Audio.h>
#include <StatusLed.h>
#include <Wifly.h>
#include <Configuration.h>
#include <eepromAddresses.h>
#include <Inbox.h>
#include <Resources.h>
#include <Settings.h>
#include <Button.h>
#include <Motion.h>
#include <Personality.h>
#include <Player.h>
#include <PusherClient.h>
#include <PusherTrajectory.h>
#include <Settings.h>
#include <ServoControl.h>
//------------------------------------------------------------------------------
#define BUFFER_SIZE 4096
//------------------------------------------------------------------------------
class Dispatcher {
public:
    Dispatcher();
    void setup();
    void loop();
//------------------------------------------------------------------------------
private:
    // Base
    char buffer[BUFFER_SIZE];
    Api api;
    Audio audio;
    SdFat sd;
    StatusLed led;
    Wifly wifly;
    // Services
    Inbox inbox;
    Configuration config;
    // Behavior
    Button button;
    Player player;
    Personality personality;
    PusherClient pusher;
    PusherTrajectory realtime;
    Motion motion;
    Settings settings;
    ServoControl control;
};
//------------------------------------------------------------------------------
#endif // DISPATCHER_H
