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
#include <Api.h>
//------------------------------------------------------------------------------
// Buffer sizes
/** Size of the temporary buffer used to read API path */
uint16_t const API_PATH_BUFFER_SIZE = 384;
//------------------------------------------------------------------------------
// API path format strings
/** Format string for an API call with the base URL and fixed arguments */
const char PROGMEM API_CALL_NO_PARAMS[] = "%S%S?%s";
/** API call with 1 additionnal key-value argument */
const char PROGMEM API_CALL_ONE_PARAM[] = "%S%S?%s&%S=%s";
/** API call with 2 additionnal key-value arguments */
const char PROGMEM API_CALL_TWO_PARAMS[] = "%S%S?%s&%S=%s&%S=%s";
/** API call with 3 additionnal key-value arguments */
const char PROGMEM API_CALL_THREE_PARAMS[] = "%S%S?%s&%S=%s&%S=%s&%S=%s";
/** API call with 4 additionnal key-value arguments */
const char PROGMEM API_CALL_FOUR_PARAMS[] = "%S%S?%s&%S=%s&%S=%s&%S=%s&%S=%s";
//------------------------------------------------------------------------------
/**
 * Construct an instance of Api.
 *
 * \param[in] wifly The Wifly object used to connect to the internet.
 * \param[out] buffer The buffer where the response will be written.
 * \param[in] bufferSize The size of the output buffer.
 * \param[in] host The remote host where the API is located.
 * \param[in] baseUrl The path of the API on the host.
 * \param[in] fixedArgs A string which will be sent with each API request.
 *
 * \note The API path needs to be in the program memory, whereas the fixed
 * arguments should be a string in the RAM.
 */
 Api::Api(Wifly &wifly, char* buffer, size_t bufferSize, PGM_P host,
        PGM_P baseUrl, char* fixedArgs) :
    HttpClient(wifly),
    JsonStream(buffer, bufferSize),
    host_(host),
    baseUrl_(baseUrl),
    fixedArgs_(fixedArgs)
{
}
//------------------------------------------------------------------------------
/**
 * Construct an instance of Api.
 *
 * \param[in] wifly The Wifly object used to connect to the internet.
 * \param[out] buffer The buffer where the response will be written.
 * \param[in] bufferSize The size of the output buffer.
 * \param[in] host The remote host where the API is located.
 * \param[in] baseUrl The path of the API on the host.
 *
 * \note The API path needs to be in the program memory, whereas the fixed
 *arguments should be a string in the RAM.
 */
 Api::Api(Wifly &wifly, char* buffer, size_t bufferSize, PGM_P host,
        PGM_P baseUrl) :
    HttpClient(wifly),
    JsonStream(buffer, bufferSize),
    host_(host),
    baseUrl_(baseUrl)
{
    fixedArgs_ = "";
}
//------------------------------------------------------------------------------
/**
 * Send a call to the API without any arguments.
 *
 * \param[in] method The method to call.
 *
 * \return The number of bytes actually received, -1 in case of failure.
 */
int Api::call(PGM_P method) {
    if (!connected()) {
        return -1;
    }
    char path[API_PATH_BUFFER_SIZE] = {0};
    snprintf_P(
        path,
        API_PATH_BUFFER_SIZE,
        API_CALL_NO_PARAMS,
        baseUrl_,
        method,
        fixedArgs_
    );
    char host[WIFLY_HOST_BUFFER_SIZE] = {0};
    strlcpy_P(host, host_, WIFLY_HOST_BUFFER_SIZE);
    int nBytes = get(buffer_, bufferSize_, host, path);
    return nBytes;
}
//------------------------------------------------------------------------------
/**
 * Send a call to the API with one argument.
 *
 * \param[in] method The method to call.
 * \param[in] key1 Name of the first parameter.
 * \param[in] value1 String value of the first parameter.
 *
 * \return The number of bytes actually received, -1 in case of failure.
 */
int Api::call(PGM_P method, PGM_P key1, const char* value1) {
    if (!connected())
        return -1;
    char path[API_PATH_BUFFER_SIZE] = {0};
    snprintf_P(
        path,
        API_PATH_BUFFER_SIZE,
        API_CALL_ONE_PARAM,
        baseUrl_,
        method,
        fixedArgs_,
        key1, value1
    );
    char host[API_PATH_BUFFER_SIZE] = {0};
    strlcpy_P(host, host_, API_PATH_BUFFER_SIZE);
    return get(buffer_, bufferSize_, host, path);
}
//------------------------------------------------------------------------------
/**
 * Send a call to the API with two arguments.
 *
 * \param[in] method The method to call.
 * \param[in] key1 Name of the first parameter.
 * \param[in] value1 String value of the first parameter.
 * \param[in] key2 Name of the second parameter.
 * \param[in] value2 String value of the second parameter.
 *
 * \return The number of bytes actually received, -1 in case of failure.
 */
int Api::call(PGM_P method, PGM_P key1, const char* value1, PGM_P key2,
    const char* value2) {
    if (!connected())
        return -1;
    char path[API_PATH_BUFFER_SIZE] = {0};
    snprintf_P(
        path,
        API_PATH_BUFFER_SIZE,
        API_CALL_TWO_PARAMS,
        baseUrl_,
        method,
        fixedArgs_,
        key1, value1,
        key2, value2
    );
    char host[API_PATH_BUFFER_SIZE] = {0};
    strlcpy_P(host, host_, API_PATH_BUFFER_SIZE);
    return get(buffer_, bufferSize_, host, path);
}
//------------------------------------------------------------------------------
/**
 * Send a call to the API with three arguments.
 *
 * \param[in] method The method to call.
 * \param[in] key1 Name of the first parameter.
 * \param[in] value1 String value of the first parameter.
 * \param[in] key2 Name of the second parameter.
 * \param[in] value2 String value of the second parameter.
 * \param[in] key3 Name of the third parameter.
 * \param[in] value3 String value of the third parameter.
 *
 * \return The number of bytes actually received, -1 in case of failure.
 */
int Api::call(PGM_P method, PGM_P key1, const char* value1, PGM_P key2,
    const char* value2, PGM_P key3, const char* value3) {
    if (!connected())
        return -1;
    char path[API_PATH_BUFFER_SIZE] = {0};
    snprintf_P(
        path,
        API_PATH_BUFFER_SIZE,
        API_CALL_THREE_PARAMS,
        baseUrl_,
        method,
        fixedArgs_,
        key1, value1,
        key2, value2,
        key3, value3
    );
    char host[API_PATH_BUFFER_SIZE] = {0};
    strlcpy_P(host, host_, API_PATH_BUFFER_SIZE);
    return get(buffer_, bufferSize_, host, path);
}
//------------------------------------------------------------------------------
/**
 * Send a call to the API with four arguments.
 *
 * \param[in] method The method to call.
 * \param[in] key1 Name of the first parameter.
 * \param[in] value1 String value of the first parameter.
 * \param[in] key2 Name of the second parameter.
 * \param[in] value2 String value of the second parameter.
 * \param[in] key3 Name of the third parameter.
 * \param[in] value3 String value of the third parameter.
 * \param[in] key4 Name of the fourth parameter.
 * \param[in] value4 String value of the fourth parameter.
 *
 * \return The number of bytes actually received, -1 in case of failure.
 */
int Api::call(PGM_P method, PGM_P key1, const char* value1, PGM_P key2,
    const char* value2, PGM_P key3, const char* value3, PGM_P key4,
    const char* value4) {
    if (!connected())
        return -1;
    char path[API_PATH_BUFFER_SIZE] = {0};
    snprintf_P(
        path,
        API_PATH_BUFFER_SIZE,
        API_CALL_FOUR_PARAMS,
        baseUrl_,
        method,
        fixedArgs_,
        key1, value1,
        key2, value2,
        key3, value3,
        key4, value4
    );
    char host[API_PATH_BUFFER_SIZE] = {0};
    strlcpy_P(host, host_, API_PATH_BUFFER_SIZE);
    return get(buffer_, bufferSize_, host, path);
}
//------------------------------------------------------------------------------
/**
 * Open a connection to the host.
 *
 * \return true is returned if a connection is successfully opened to the host.
 * Otherwise, false is returned.
 */
bool Api::connect() {
    char host[WIFLY_HOST_BUFFER_SIZE] = {0};
    strlcpy_P(host, host_, WIFLY_HOST_BUFFER_SIZE);
    return HttpClient::connect(host);
}
//------------------------------------------------------------------------------
/** Check whether the client is connected to the host */
bool Api::connected() {
    return wifly_->connectedTo_P(host_);
}
//------------------------------------------------------------------------------
/**
 * Provide a pointer to a null-terminated string which will automatically be
 * appended to any call made by the API instance.
 *
 * \param[in] data The arguments added to each API call.
 */
void Api::setFixedArgs(char* data) {
    fixedArgs_ = data;
}
