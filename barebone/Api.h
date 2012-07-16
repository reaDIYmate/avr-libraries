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
#ifndef API_H
#define API_H
/**
 * \file
 * \brief Api class to manage communications with webservices.
 */
#include <avr/pgmspace.h>
#include <HttpClient.h>
#include <JsonStream.h>
//------------------------------------------------------------------------------
/**
 * \class Api
 * \brief Send formatted requests to a an API and parse the JSON responses.
 */
class Api : public HttpClient, public JsonStream {
public:
    Api(Wifly &wifly, char* buffer, size_t bufferSize, PGM_P host,
        PGM_P baseUrl);
    Api(Wifly &wifly, char* buffer, size_t bufferSize, PGM_P host,
        PGM_P baseUrl, char* fixedArgs);
    int call(PGM_P method);
    int call(PGM_P method, PGM_P key1, const char* value1);
    int call(PGM_P method, PGM_P key1, const char* value1, PGM_P key2,
        const char* value2);
    int call(PGM_P method, PGM_P key1, const char* value1, PGM_P key2,
        const char* value2, PGM_P key3, const char* value3);
    int call(PGM_P method, PGM_P key1, const char* value1, PGM_P key2,
        const char* value2, PGM_P key3, const char* value3, PGM_P key4,
        const char* value4);
    bool connect();
    bool connected();
    void setFixedArgs(char* data);
    using HttpClient::disconnect;
//------------------------------------------------------------------------------
private:
    /** Base URL of the API calls */
    PGM_P baseUrl_;
    /** Host where the API resides */
    PGM_P host_;
    /** Arguments added to each API call*/
    char* fixedArgs_;
};

#endif // API_H
