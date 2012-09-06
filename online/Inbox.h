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
#ifndef INBOX_H
#define INBOX_H
/**
 * \file
 * \brief Inbox class to fetch and process incoming messages.
 */
#include <avr/pgmspace.h>
#include <Api.h>
#include <Pusherclient.h>
//------------------------------------------------------------------------------
// Inbox message types
/** Invalid message or timeout */
int const INBOX_NOTHING = -1;
/** Ping request from the server - no actual message */
int const INBOX_PING = 0;
/** Start remote control mode */
int const INBOX_START_REMOTE = 1;
//------------------------------------------------------------------------------
/**
 * \class Inbox
 * \brief Poll the inbox and handle each message according to its type.
 */
class Inbox {
public:
    Inbox(Api &api, PusherClient &pusher);
    bool enterPushMode();
    int getIntegerByName_P(PGM_P key) {return stream_->getIntegerByName_P(key);}
    int getMessage();
    int getStringByName_P(PGM_P key, char* buffer, size_t length) {
       return stream_->getStringByName_P(key, buffer, length);}
    bool leavePushMode();
    bool updatePusherChannel();
//------------------------------------------------------------------------------
private:
    /** The JsonStream object used for JSON parsing */
    JsonStream* stream_;
    /** Instance of Api used to access the reaDIYmate API */
    Api* api_;
    /** Pusher client used to receive realtime messages */
    PusherClient* pusher_;
    /** Name of the channel used on Pusher */
    char channel_[21];
    /** Cache the ID of the last message sender */
    char senderId_[13];
};

#endif // INBOX_H
