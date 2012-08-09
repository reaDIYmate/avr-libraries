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
#ifndef PUSHER_CLIENT_H
#define PUSHER_CLIENT_H
/**
 * \file
 * \brief PusherClient class.
 */
#include <avr/pgmspace.h>
#include <JsonStream.h>
#include <Sha256.h>
#include <Wifly.h>
//------------------------------------------------------------------------------
/**
 * \class PusherClient
 * \brief Exchange Websockets messages via Pusher.
 */
class PusherClient : public JsonStream {
public:
    PusherClient(Wifly &wifly, char* buffer, size_t bufferSize);
    bool authenticate();
    bool connect();
    bool connected();
    void disconnect();
    bool hasNextEvent();
    bool ping();
    void pong();
    bool subscribe(const char* channel, char* auth = NULL);
    //------------------------------------------------------------------------------
    /*
     * Update the channel.
     *
     * \param[in] channel The channel name.
     */
    void setChannel(const char* channel) {channel_ = channel;}
    /*
     * Update the public key.
     *
     * \param[in] key The public key of the Pusher application.
     */
    void setKey(const char* key) {key_ = key;}
    /*
     * Update the secret key.
     *
     * \param[in] secret The secret key of the Pusher application.
     */
     void setSecret(const char* secret) {secret_ = secret;}
    using JsonStream::getIntegerByName;
//------------------------------------------------------------------------------
private:
    void generateAuth(const char* key, const char* secret, const char* channel,
        char* auth);
    void send(const char* data);
    void send_P(PGM_P data);
    /** Public key for the Pusher application */
    const char* key_;
    /** Private key for the Pusher application */
    const char* secret_;
    /** Active channel on the Pusher application */
    const char* channel_;
    /** Socket ID provided by Pusher */
    char socketId_[16];
    /** WiFly module manager */
    Wifly* wifly_;

};

#endif // PUSHER_CLIENT_H
