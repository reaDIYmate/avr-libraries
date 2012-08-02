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
#include <PusherClient.h>
//------------------------------------------------------------------------------
// Pusher strings
/** Hash format string */
const char PROGMEM PUSHER_HASH_STRING[] = "%s:%s";
/** Authentication token format string */
const char PROGMEM PUSHER_AUTH_TOKEN[] = "%s:%s";
/** Pusher host */
const char PROGMEM PUSHER_HOST[] = "ws.pusherapp.com";
/** Pusher socket upgrade confirmation */
const char PROGMEM PUSHER_CONNECTED[] = "pusher:connection_established";
/** Pusher JSON key for the socket ID */
const char PROGMEM PUSHER_KEY_SOCKET_ID[] = "socket_id";
/** Pusher ping event */
const char PROGMEM PUSHER_PING[] = "pusher:ping";
/** Pusher subscription confirmation */
const char PROGMEM PUSHER_SUBSCRIBED[] =
    "pusher_internal:subscription_succeeded";
/** Pusher websocket request format string */
const char PROGMEM PUSHER_WEBSOCKET_REQUEST[] =
    "GET /app/%s?client=js&version=1.11.2 HTTP/1.1\r\n"
    "Upgrade: Websocket\r\n"
    "Connection: Upgrade\r\n"
    "Host: %S\r\n"
    "Origin: reaDIYmate/0.3\r\n"
    "\r\n";
/** Pusher channel subscription format string */
const char PROGMEM PUSHER_SUBSCRIBE[] =
    "{"
    "\"event\": \"pusher:subscribe\","
    "\"data\": {\"channel\": \"%s\"}"
    "}";
/** Pusher channel subscription format string with authentication */
const char PROGMEM PUSHER_SUBSCRIBE_AUTH[] =
    "{"
    "\"event\": \"pusher:subscribe\","
    "\"data\": {\"channel\": \"%s\", \"auth\":\"%s\"}"
    "}";
const char PROGMEM PUSHER_PONG[] =
    "{"
    "\"event\": \"pusher:pong\","
    "\"data\": {}"
    "}";
char const PUSHER_STARTCHAR = 0x00;
char const PUSHER_ENDCHAR = 0xFF;
uint8_t const PUSHER_HASH_LENGTH = 32;
//------------------------------------------------------------------------------
/**
 * Construct an instance of PusherClient.
 *
 * \param[in] wifly
 * \param[in] buffer
 * \param[in] bufferSize
 */
PusherClient::PusherClient(Wifly &wifly, char* buffer, size_t bufferSize) :
    JsonStream(buffer, bufferSize),
    wifly_(&wifly),
    key_(NULL),
    secret_(NULL),
    channel_(NULL)
{
}
//------------------------------------------------------------------------------
/**
 * Run the authentication process for the current connection on a given channel.
 *
 * \param[in] channel A string representing the channel name.
 *
 * \return true is returned if the authentication is accepted, false if it fails.
 */
bool PusherClient::authenticate(const char* channel) {
    if (channel != NULL)
        channel_ = channel;
    if (key_ == NULL || secret_ == NULL || channel_ == NULL)
        return false;
    if (!connected())
        return false;
    char auth[128];
    generateAuth(key_, secret_, channel_, auth);
    return subscribe(channel_, auth);
}
//------------------------------------------------------------------------------
/** Create a websocket connection to the Pusher system.
 *
 * \return true is returned if the connection is established, false if it fails.
 */
bool PusherClient::connect() {
    // open a TCP socket to Pusher
    memset(buffer_, 0x00, bufferSize_);
    strlcpy_P(buffer_, PUSHER_HOST, bufferSize_);
    if (!wifly_->connect(buffer_))
        goto fail;

    // request a socket upgrade to websocket
    memset(buffer_, 0x00, bufferSize_);
    snprintf_P(
        buffer_,
        bufferSize_,
        PUSHER_WEBSOCKET_REQUEST,
        key_,
        PUSHER_HOST
    );
    wifly_->clear();
    if (!wifly_->print(buffer_))
        goto fail;
    wifly_->flush();
    // read server response
    if (!wifly_->awaitResponse())
        goto fail;
    if (!hasNextEvent())
        goto fail;

    // look for socket upgrade confirmation
    if (!find_P(PUSHER_CONNECTED))
        goto fail;

    // parse socket ID
    memset(socketId_, 0x00, 16);
    if (!getStringByName_P(PUSHER_KEY_SOCKET_ID, socketId_, 16))
        goto fail;

    return true;

    fail:
    wifly_->disconnect();
    return false;
}
//------------------------------------------------------------------------------
/**
 * Check whether the client is connected to Pusher
 *
 * \return true is returned if the client is connected, false otherwise.
 */
bool PusherClient::connected() {
    return wifly_->connectedTo_P(PUSHER_HOST);
}
//------------------------------------------------------------------------------
/** Close any open connection. */
void PusherClient::disconnect() {
    wifly_->disconnect();
}
//------------------------------------------------------------------------------
/**
 * Create an authentication signature for the given channel.
 *
 * \param[in] key Public key for the Pusher application.
 * \param[in] secret Private key for the Pusher application.
 * \param[in] channel The channel to use in the Pusher application.
 * \param[out] auth Pointer to the desired location of the auth string.
 */
void PusherClient::generateAuth(const char* key, const char* secret,
    const char* channel, char* auth) {
    // copy secret from EEPROM to SRAM
    memset(buffer_, 0x00, bufferSize_);
    strncpy(buffer_, secret, strlen(secret));

    // initialize SHA256
    Sha256.initHmac((uint8_t*)buffer_, strlen(secret));

    // combine socket ID and channel into one string
    memset(buffer_, 0x00, bufferSize_);
    snprintf_P(
        buffer_,
        bufferSize_,
        PUSHER_HASH_STRING,
        socketId_,
        channel
    );
    // generate HMAC-SHA256 digest
    Sha256.print(buffer_);
    uint8_t* hash = Sha256.resultHmac();

    // copy result to signature buffer
    char signature[2*PUSHER_HASH_LENGTH+1];
    memset(signature, 0x00, 2*PUSHER_HASH_LENGTH+1);
    for (uint8_t i = 0; i < PUSHER_HASH_LENGTH; i++) {
        sprintf(signature+2*i, "%02x", hash[i]);
    }

    // combine app key and signature into output buffer
    memset(auth, 0x00, 128);
    snprintf_P(
        auth,
        128,
        PUSHER_AUTH_TOKEN,
        key,
        signature
    );
}
//------------------------------------------------------------------------------
/**
 * Check for a Pusher event.
 *
 * \return true is returned if a new event is received from the Pusher
 * websocket, false otherwise.
 */
bool PusherClient::hasNextEvent() {
    if (!wifly_->available())
        return false;
    if (!wifly_->find(PUSHER_STARTCHAR))
        return false;
    memset(buffer_, 0x00, bufferSize_);
    int nBytes = wifly_->readBytesUntil(PUSHER_ENDCHAR, buffer_, bufferSize_);
    if (nBytes > 0) {
        index_ = 0;
        return true;
    }
    else
        return false;
}
//------------------------------------------------------------------------------
/**
 * Look for an incoming ping event from Pusher
 *
 * \return true is returned if a ping event if received, false otherwise.
 */
bool PusherClient::ping() {
    return find_P(PUSHER_PING);
}
//------------------------------------------------------------------------------
/** Send a pong event to Pusher */
void PusherClient::pong() {
    send_P(PUSHER_PONG);
}
//------------------------------------------------------------------------------
/*
 * Send a string using websocket data framing
 *
 * \param[in] data The null-terminated string to send.
 */
void PusherClient::send(const char* data) {
    wifly_->write(PUSHER_STARTCHAR);
    wifly_->print(data);
    wifly_->write(PUSHER_ENDCHAR);
    wifly_->flush();
}
//------------------------------------------------------------------------------
/*
 * Send a Flash-based string using websocket data framing
 *
 * \param[in] data The progmem string to send.
 */
void PusherClient::send_P(PGM_P data) {
    wifly_->write(PUSHER_STARTCHAR);
    wifly_->write_P(data);
    wifly_->write(PUSHER_ENDCHAR);
    wifly_->flush();
}
//------------------------------------------------------------------------------
/*
 * Update the Pusher parameters
 *
 * \param[in] key The public key of the Pusher application.
 * \param[in] secret The secret key of the Pusher application.
 * \param[in] channel The channel name.
 */
void PusherClient::setConfig(const char* key, const char* secret,
    const char* channel) {
    key_ = key;
    secret_ = secret;
    channel_ = channel;
}

//------------------------------------------------------------------------------
/**
 * Subscribe to a channel using authentication
 *
 * \param[in] channel The name of the desired channel.
 * \param[in] auth The authentication token to use.
 *
 * \return true is returned if the subscription is successfull, false otherwise.
 */
bool PusherClient::subscribe(const char* channel, char* auth) {
    memset(buffer_, 0x00, bufferSize_);
    if (auth == NULL) {
        snprintf_P(
            buffer_,
            bufferSize_,
            PUSHER_SUBSCRIBE,
            channel
        );
    }
    else {
        snprintf_P(
            buffer_,
            bufferSize_,
            PUSHER_SUBSCRIBE_AUTH,
            channel,
            auth
        );
    }
    wifly_->clear();
    send(buffer_);
    if (!wifly_->awaitResponse())
        return false;
    if (!hasNextEvent())
        return false;
    return find_P(PUSHER_SUBSCRIBED);
}
