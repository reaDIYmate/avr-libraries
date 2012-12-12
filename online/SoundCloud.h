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
#ifndef SOUNDCLOUD_H
#define SOUNDCLOUD_H
/**
 * \file
 * \brief SoundCloud class to download files via HTTP.
 */
#include <Action.h>
#include <HttpClient.h>
#include <SdFat.h>
//------------------------------------------------------------------------------
/**
 * \class SoundCloud
 * \brief Fetch files from the SoundCloud server and save them to the SD card.
 */
class SoundCloud : public HttpClient, public SdFile, public Action {
public:
    SoundCloud(Api &api, Wifly &wifly, char* buffer, size_t bufferSize,
        SdFat &sd, uint8_t sdChipSelectPin, Settings &settings_, PGM_P owner,
        PGM_P actionEnabled, PGM_P downloadsEnabled);
    PGM_P directory();
    bool download();
    bool downloadsEnabled();
    const char* filepath();
//------------------------------------------------------------------------------
private:
    typedef struct {
        char* host;
        char* path;
    } url_t;
    typedef struct {
        uint32_t size;
        uint16_t nParts;
    } download_t;
    uint32_t getLengthFromHeader(char* buffer, size_t bufferSize, url_t& url);
    int getPart(uint32_t partNo, url_t& url, download_t &download);
    bool initializeDownload();
    bool renewDownloadUrl(url_t &url);
    /** An SdFat object to manage the SD card */
    SdFat* sd_;
    /** A buffer to hold the path to recently downloaded files */
    char filepath_[32];
    /** Buffer to use during downloads */
    char* buffer_;
    /** Size of the buffer */
    size_t bufferSize_;
    /** SD card Chip Select pin */
    const uint8_t sdChipSelectPin_;
    /** The name of the parameter to pass to API calls */
    PGM_P owner_;
    /** The alert flag in the settings */
    PGM_P downloadsEnabled_;
};

#endif // SOUNDCLOUD_H
