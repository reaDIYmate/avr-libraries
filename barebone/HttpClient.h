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
#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
/**
 * \file
 * \brief HttpClient class.
 */
#include <avr/pgmspace.h>
#include <Wifly.h>
//------------------------------------------------------------------------------
// HTTP request flags
/** Generate a HEAD request */
uint8_t const F_HEAD = 0x01;
/** Generate a GET request */
uint8_t const F_GET = 0x02;
/** Generate a POST request */
uint8_t const F_POST = 0x04;
/** Use a persistent HTTP connection */
uint8_t const F_KEEP_ALIVE = 0x08;
/** Close the HTTP connection after the request */
uint8_t const F_CLOSE = 0x10;
//------------------------------------------------------------------------------
/**
 * \class HttpClient
 * \brief Basic HTTP client.
 */
class HttpClient {
public:
    /**
     * Construct an instance of HttpClient.
     *
     * \param[in] wifly The Wifly object to use for communications.
     */
    explicit HttpClient(Wifly &wifly) : wifly_(&wifly) {}
    bool connect(const char* host);
    void disconnect();
    int get(char* buffer, size_t bufferSize, const char* host,
        const char* path);
    uint32_t getContentLength(char* buffer, size_t bufferSize, const char* host,
        const char* path);
    bool getRange(char* buffer, size_t bufferSize, const char* host,
        const char* path, uint32_t firstByte, uint32_t lastByte);
    int post(char* buffer, size_t bufferSize, const char* host,
        const char* path, const char* content);
//------------------------------------------------------------------------------
protected:
    bool createGetRequest(char* buffer, size_t bufferSize, const char* host,
        const char* path, uint8_t flags = (F_HEAD | F_CLOSE),
        uint32_t firstByte = 0, uint32_t lastByte = 0);
    bool createPostRequest(char* buffer, size_t bufferSize, const char* host,
        const char* path,  const char* content, uint8_t flags = (F_HEAD | F_CLOSE));
    /** WiFly module object */
    Wifly* wifly_;
};

#endif // HTTP_CLIENT_H
