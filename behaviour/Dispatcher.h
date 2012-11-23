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
#include <Email.h>
#include <Inbox.h>
#include <Resources.h>
#include <Button.h>
#include <Gmail.h>
#include <Facebook.h>
#include <Foursquare.h>
#include <Motion.h>
#include <Personality.h>
#include <Player.h>
#include <PusherClient.h>
#include <PusherTrajectory.h>
#include <Rss.h>
#include <Settings.h>
#include <ServoControl.h>
#include <SoundCloud.h>
#include <Twitter.h>
//------------------------------------------------------------------------------
#define BUFFER_SIZE 2048
//------------------------------------------------------------------------------
class Dispatcher {
public:
    Dispatcher();
    void setup();
    void loop();
    void play(const char* sound, const char* motion);
    void play_P(PGM_P sound, PGM_P motion);
//------------------------------------------------------------------------------
private:
    void die(__FlashStringHelper* message);
    // Base
    char buffer[BUFFER_SIZE];
    Api api;
    Audio audio;
    SdFat sd;
    StatusLed led;
    Wifly wifly;
    // Services
    Configuration config;
    Facebook facebook;
    Foursquare foursquare;
    Email email;
    Gmail gmail;
    Inbox inbox;
    Rss rss;
    Resources resources;
    Settings settings;
    SoundCloud soundcloud;
    Twitter twitter;
    // Behavior
    Button button;
    Player player;
    Personality personality;
    PusherClient pusher;
    PusherTrajectory realtime;
    Motion motion;
    ServoControl control;
};
//------------------------------------------------------------------------------
#endif // DISPATCHER_H
