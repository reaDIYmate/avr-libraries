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
#ifndef DOWNLOAD_H
#define DOWNLOAD_H
/**
 * \file
 * \brief Download class to download files via HTTP.
 */
#include <HttpClient.h>
#include <SdFat.h>
//------------------------------------------------------------------------------
/**
 * \class Download
 * \brief Retrieve files from a remote server and save them to the SD card.
 */
class Download : public HttpClient, public SdFile {
public:
    Download(Wifly &wifly, char* buffer, size_t bufferSize, PGM_P host,
        SdFat &sd, uint8_t sdChipSelectPin);
    bool connect();
    bool connected();
    bool save(const char* path, const char* localName);
//------------------------------------------------------------------------------
protected:
    /** Buffer to use during downloads */
    char* buffer_;
    /** Size of the buffer */
    size_t bufferSize_;
//------------------------------------------------------------------------------
private:
    /** API host */
    PGM_P host_;
    /** SD card Chip Select pin */
    const uint8_t sdChipSelectPin_;
    /** An SdFat object to manage the SD card */
    SdFat* sd_;
};

#endif // DOWNLOAD_H
