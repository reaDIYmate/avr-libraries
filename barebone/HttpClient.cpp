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
#include <HttpClient.h>
//------------------------------------------------------------------------------
/** HTTP GET request format string */
const char PROGMEM HTTP_REQUEST_GET[] =
    // command line made of the GET command followed by a URL
    "GET %s HTTP/1.1\r\n"
    // domain name of the website
    "Host: %s\r\n"
    // user agent used by the device
    "User-Agent: reaDIYmate/0.3\r\n"
    // the connection to the host can be kept alive between successive requests
    "Connection: %S\r\n"
    // HTTP headers end with "\r\n"
    "\r\n";
/** HTTP partial GET request format string */
const char PROGMEM HTTP_REQUEST_PARTIAL_GET[] =
    // command line made of the GET command followed by a URL
    "GET %s HTTP/1.1\r\n"
    // domain name of the website
    "Host: %s\r\n"
    // user agent used by the device
    "User-Agent: reaDIYmate/0.3\r\n"
    // the connection to the host can be kept alive between successive requests
    "Connection: %S\r\n"
    // specifying a range allows partial GET requests
    "Range: bytes=%lu-%lu\r\n"
    // HTTP headers end with "\r\n"
    "\r\n";
/** HTTP HEAD request format string */
const char PROGMEM HTTP_REQUEST_HEAD[] =
    // command line made of the GET command followed by a URL
    "HEAD %s HTTP/1.1\r\n"
    // domain name of the website
    "Host: %s\r\n"
    // user agent used by the device
    "User-Agent: reaDIYmate/0.3\r\n"
    // the connection to the host can be kept alive between successive requests
    "Connection: %S\r\n"
    // HTTP headers end with "\r\n"
    "\r\n";
const char PROGMEM HTTP_REQUEST_POST[] =
    // command line made of the GET command followed by a URL
    "POST %s HTTP/1.1\r\n"
    // domain name of the website
    "Host: %s\r\n"
    // user agent used by the device
    "User-Agent: dsn/1.0\r\n"
    // the connection to the host can be kept alive between successive requests
    "Connection: %S\r\n"
    // the length of the content
    "Content-Length: %d\r\n"
    // HTTP headers end with "\r\n"
    "\r\n"
    // the data to post with the request
    "%s";
/** Chunked transfer mode */
const char PROGMEM HTTP_CHUNKED[] = "Transfer-Encoding: chunked";
/** Regular connection type */
const char PROGMEM HTTP_FIELD_CLOSE[] = "Close";
/** Content length field in the HTTP header */
const char PROGMEM HTTP_CONTENT_LENGTH[] = "Content-Length: ";
/** End of line */
const char PROGMEM HTTP_CRLF[] = "\r\n";
/** End of header */
const char PROGMEM HTTP_END_OF_HEADER[] = "\r\n\r\n";
/** Persistent connection type */
const char PROGMEM HTTP_FIELD_KEEP_ALIVE[] = "Keep-Alive";
/** Partial content status code */
const char PROGMEM HTTP_PARTIAL[] = "206 Partial Content";
/** Size of the temporary buffer used for sscanf() */
uint8_t const SSCANF_BUFFER_SIZE = 8;
//------------------------------------------------------------------------------
/** Open a persistent connection to the host */
bool HttpClient::connect(const char* host) {
    wifly_->reset();
    return wifly_->connect(host);
}
//------------------------------------------------------------------------------
/** Close any open connection */
void HttpClient::disconnect() {
    wifly_->disconnect();
}
//------------------------------------------------------------------------------
/**
 * Create an HTTP request and store it in the provided buffer.
 *
 * \param[out] buffer The buffer where the request will be stored.
 * \param[in] bufferSize The size of the output buffer.
 * \param[in] host The remote host where the resource is located
 * \param[in] path The path of the desired resource on the host.
 * \param[in] flags Flags used to determine the request type and connection
 * mode (see values above).
 * \param[in] firstByte Index of the first byte of the requested range.
 * \param[in] lastByte Index of the last byte of the requested range.
 */
bool HttpClient::createGetRequest(char* buffer, size_t bufferSize,
    const char* host, const char* path, uint8_t flags, uint32_t firstByte,
    uint32_t lastByte) {
    // check flags
    if ((flags & F_HEAD) == (flags & F_GET))
        return false;
    else if ((flags & F_KEEP_ALIVE) == (flags & F_CLOSE))
        return false;
    // determine request type
    PGM_P request;
    if (flags & F_HEAD)
        request = HTTP_REQUEST_HEAD;
    else if (flags & F_GET) {
        bool partial = !(firstByte == 0 && lastByte == 0);
        request = partial ? HTTP_REQUEST_PARTIAL_GET : HTTP_REQUEST_GET;
    }
    // determine connection type
    bool keepAlive = (flags & F_KEEP_ALIVE);
    PGM_P connection = keepAlive ? HTTP_FIELD_KEEP_ALIVE : HTTP_FIELD_CLOSE;
    // print the request to the buffer
    memset(buffer, 0x00, bufferSize);
    snprintf_P(
        buffer,
        bufferSize,
        request,
        path,
        host,
        connection,
        firstByte,
        lastByte
    );
    return true;
}
//------------------------------------------------------------------------------
bool HttpClient::createPostRequest(char* buffer, size_t bufferSize,
    const char* host, const char* path, const char* content, uint8_t flags) {
    // check flags
    if ((flags & F_KEEP_ALIVE) == (flags & F_CLOSE)) {
        return false;
    }
    else if ((flags & F_HEAD) == (flags & F_POST)) {
        return false;
    }
    // determine request type
    PGM_P request;
    if (flags & F_HEAD) {
        request = HTTP_REQUEST_HEAD;
    }
    else if (flags & F_POST) {
        request = HTTP_REQUEST_POST;
    }
    // determine connection type
    bool keepAlive = (flags & F_KEEP_ALIVE);
    PGM_P connection = keepAlive ? HTTP_FIELD_KEEP_ALIVE : HTTP_FIELD_CLOSE;
    // print the request to the buffer
    memset(buffer, 0x00, bufferSize);
    snprintf_P(
        buffer,
        bufferSize,
        request,
        path,
        host,
        connection,
        strlen(content),
        content
    );
    return true;
}
//------------------------------------------------------------------------------
/**
 * Connect to host and send a GET request to retrieve the given URL.
 *
 * \param[out] buffer The buffer where the response will be written.
 * \param[in] bufferSize The size of the output buffer.
 * \param[in] host The remote host where the resource is located.
 * \param[in] path The path of the desired resource on the host.
 *
 * \return The number of bytes actually received, -1 in case of failure.
 *
 * \note host can be either the domain name or the IP address of the host.
 */
int HttpClient::get(char* buffer, size_t bufferSize, const char* host,
    const char* path) {
    // generate the HTTP request
    if (!createGetRequest(buffer, bufferSize, host, path,
        (F_GET | F_KEEP_ALIVE))) {
        return -1;
    }

    // try to send the request and wait for a response from the host
    wifly_->clear();
    if (!wifly_->print(buffer))
        return -1;
    wifly_->flush();

    // clear the buffer since it still holds the HTTP request
    memset(buffer, 0x00, bufferSize);
    if (!wifly_->awaitResponse())
        return -1;

    // determine transfer mode
    bool chunked = wifly_->findUntil_P(HTTP_CHUNKED, HTTP_END_OF_HEADER);
    if (chunked) {
        // skip the rest of the header
        if (!wifly_->find_P(HTTP_END_OF_HEADER))
            return -1;
        // process successive chunks
        uint16_t index = 0;
        while (index < bufferSize - 1) {
            char parseBuffer[SSCANF_BUFFER_SIZE];
            // read the line containing the chunk size
            int nBytes = wifly_->readBytesUntil_P(HTTP_CRLF, parseBuffer,
                SSCANF_BUFFER_SIZE);
            // chunk size should be at least one digit followed by \r\n
            if (nBytes < 1 + strlen_P(HTTP_CRLF))
                return -1;
            // erase the \r\n
            memset(parseBuffer + nBytes - 2, 0x00, 2);
            // parse chunk size
            uint16_t chunkSize = 0;
            sscanf(parseBuffer, "%x", &chunkSize);
            if (chunkSize < 0 || chunkSize > bufferSize) {
                return -1;
            }
            if (chunkSize == 0)
                break;
            // download chunk
            nBytes = wifly_->readBytes(buffer + index, chunkSize);
            if (nBytes != chunkSize)
                return -1;
            index += chunkSize;
            // skip end of line
            if (!wifly_->find_P(HTTP_CRLF))
                return -1;
        }
        return index;
    }
    else {
        int nBytes = wifly_->readBytes(buffer, bufferSize);
        return nBytes;
    }
}
//------------------------------------------------------------------------------
/**
 * Send a HEAD request to retrieve the size of the resource located at the
 * given URL.
 * \param[out] buffer The buffer where the response will be written.
 * \param[in] bufferSize The size of the output buffer.
 * \param[in] host The remote host where the resource is located.
 * \param[in] path The path of the desired resource on the host.
 */
uint32_t HttpClient::getContentLength(char* buffer, size_t bufferSize,
    const char* host, const char* path) {
    if (!createGetRequest(buffer, bufferSize, host, path,
        (F_HEAD | F_KEEP_ALIVE)))
        return 0;
    wifly_->clear();
    if (!wifly_->print(buffer))
        return 0;
    wifly_->flush();
    memset(buffer, 0x00, bufferSize);
    if (!wifly_->awaitResponse())
        return 0;
    if (!wifly_->findUntil_P(HTTP_CONTENT_LENGTH, HTTP_END_OF_HEADER))
        return 0;
    uint8_t nBytes = wifly_->readBytesUntil_P(HTTP_CRLF, buffer, 32);
    return atol(buffer);
}
//------------------------------------------------------------------------------
/**
 * Send a GET request to retrieve a byte range from the given URL.
 *
 * \param[out] buffer The buffer where the response will be written.
 * \param[in] bufferSize The size of the output buffer.
 * \param[in] host The remote host where the resource is located.
 * \param[in] path The path of the desired resource on the host.
 * \param[in] firstByte Index of the first byte of the requested range.
 * \param[in] lastByte Index of the last byte of the requested range.
 *
 * \return true is returned if the right amount of data is successfully
 * received, false in case of failure.
 *
 * \note host can be either the domain name or the IP address of the host.
 */
bool HttpClient::getRange(char* buffer, size_t bufferSize, const char* host,
    const char* path, uint32_t firstByte, uint32_t lastByte) {
    // generate the HTTP request
    if (!createGetRequest(buffer, bufferSize, host, path,
        (F_GET | F_KEEP_ALIVE), firstByte, lastByte)) {
        return false;
    }

    // try to send the request and wait for a response from the host
    wifly_->clear();
    if (!wifly_->print(buffer))
        return false;
    wifly_->flush();
    // clear the buffer since it still holds the HTTP request
    memset(buffer, 0x00, bufferSize);

    if (!wifly_->awaitResponse())
        return false;

    // look for the right HTTP status code in the response header
    if (!wifly_->findUntil_P(HTTP_PARTIAL, HTTP_END_OF_HEADER))
        return false;
    // skip the rest of the header
    if (!wifly_->find_P(HTTP_END_OF_HEADER))
        return false;

    // write incoming data
    int nBytes = wifly_->readBytes(buffer, bufferSize);
    return (nBytes == (lastByte - firstByte + 1));
}
//------------------------------------------------------------------------------
/**
 * Send a POST request with data to the given URL.
 *
 * \param[out] buffer The buffer where the response will be written.
 * \param[in] bufferSize The size of the output buffer.
 * \param[in] host The remote host where the resource is located.
 * \param[in] path The path of the desired resource on the host.
 *
 * \return The number of bytes actually received, -1 in case of failure.
 *
 * \note host can be either the domain name or the IP address of the host.
 */
int HttpClient::post(char* buffer, size_t bufferSize, const char* host,
    const char* path, const char* content) {
    // generate the HTTP request
    if (!createPostRequest(buffer, bufferSize, host, path, content,
        (F_POST | F_KEEP_ALIVE))) {
        return -1;
    }
    // try to send the request and wait for a response from the host
    wifly_->clear();
    if (!wifly_->print(buffer))
        return -1;
    wifly_->flush();

    // clear the buffer since it still holds the HTTP request
    memset(buffer, 0x00, bufferSize);
    if (!wifly_->awaitResponse())
        return -1;

    // skip the headers
    if (!wifly_->find_P(HTTP_END_OF_HEADER))
        return -1;

    int nBytes = wifly_->readBytes(buffer, bufferSize);
    return nBytes;
}
