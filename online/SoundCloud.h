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
#include <Api.h>
#include <HttpClient.h>
#include <SdFat.h>
#include <Settings.h>
//------------------------------------------------------------------------------
/**
 * \class SoundCloud
 * \brief Retrieve files from a remote server and save them to the SD card.
 */
class SoundCloud : public HttpClient, public SdFile {
public:
    SoundCloud(Api &api, Wifly &wifly, char* buffer, size_t bufferSize,
        SdFat &sd, uint8_t sdChipSelectPin, Settings &settings_, PGM_P owner,
        PGM_P action);
    bool download(PGM_P folder);
    bool enabled() { return strcmp("1", settings_->getByName(action_)) == 0; }

//------------------------------------------------------------------------------
protected:
    /** Buffer to use during downloads */
    char* buffer_;
    /** Size of the buffer */
    size_t bufferSize_;
//------------------------------------------------------------------------------
private:
    /** Instance of Api used to access the reaDIYmate API */
    Api* api_;
    /** An SdFat object to manage the SD card */
    SdFat* sd_;
    Settings* settings_;
    /** The name of the parameter to pass to API calls */
    PGM_P owner_;
    PGM_P action_;
    /** SD card Chip Select pin */
    const uint8_t sdChipSelectPin_;
};

#endif // SOUNDCLOUD_H
