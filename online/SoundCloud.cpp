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
#include <SoundCloud.h>
//------------------------------------------------------------------------------
/** Size of the temporary buffer used to read file host */
uint8_t const HOST_BUFFER_SIZE = 128;
/** Size of the temporary buffer used to read file path */
uint16_t const PATH_BUFFER_SIZE = 512;
/** Name of the directory where SoundCloud tracks are stored */
const char PROGMEM SOUNDCLOUD_DIRECTORY[] = "SNDCLD";
/** API method used to obtain download links for the tracks */
const char PROGMEM SOUNDCLOUD_API_METHOD[] = "soundcloud/update";
/** Key for the host name in the API response */
const char PROGMEM SOUNDCLOUD_HOST_KEY[] = "host";
/** Key for the file name in the API response */
const char PROGMEM SOUNDCLOUD_NAME_KEY[] = "name";
/** Key for the track owner in the API call */
const char PROGMEM SOUNDCLOUD_OWNER_KEY[] = "owner";
/** Key for the file path in the API response */
const char PROGMEM SOUNDCLOUD_URL_KEY[] = "url";
/** Content range field in the HTTP header */
const char PROGMEM HTTP_CONTENT_RANGE[] = "Content-Range: bytes 0-1/";
/** End of line */
const char PROGMEM HTTP_CRLF[] = "\r\n";
/** End of header */
const char PROGMEM HTTP_END_OF_HEADER[] = "\r\n\r\n";
//------------------------------------------------------------------------------
/**
 * Construct an instance of SoundCloud.
 *
 * \param[in] wifly The Wifly object to use for communications
 * \param[in] buffer The Buffer to use for downloading.
 * \param[in] bufferSize The size of the buffer.
 * \param[in] host The remote host where the file is located.
 * \param[in] sd A SdFat object
 * \param[in] sdChipSelectPin The AVR pin connected to CS on the SD card.
 */
SoundCloud::SoundCloud(Api &api, Wifly &wifly, char* buffer, size_t bufferSize,
    SdFat &sd, uint8_t sdChipSelectPin, Settings &settings, PGM_P owner,
    PGM_P actionEnabled, PGM_P downloadsEnabled) :
    Action(api, settings, actionEnabled),
    HttpClient(wifly),
    buffer_(buffer),
    bufferSize_(bufferSize),
    sd_(&sd),
    sdChipSelectPin_(sdChipSelectPin),
    owner_(owner),
    downloadsEnabled_(downloadsEnabled)
{
}
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the directory where SoundCloud tracks are stored.
 *
 * \return the directory is returned as a Flash-based, 8.3 directory name.
 */
PGM_P SoundCloud::directory() {
    return SOUNDCLOUD_DIRECTORY;
}
//------------------------------------------------------------------------------
/**
 * Download a track from SoundCloud.
 *
 * \return This method returns true if the download is successfull, false if it
 * fails.
 */
bool SoundCloud::download() {
    if (!initializeDownload())
        return false;
    // parse file host and path
    char host[HOST_BUFFER_SIZE] = {0};
    if (api_->getStringByName_P(SOUNDCLOUD_HOST_KEY, host,
        HOST_BUFFER_SIZE) <= 0) {
        return false;
    }
    char path[PATH_BUFFER_SIZE] = {0};
    if (api_->getStringByName_P(SOUNDCLOUD_URL_KEY, path,
        PATH_BUFFER_SIZE) <= 0) {
        return false;
    }
    url_t url = {host, path};
    // connect to file host
    if (!connect(host))
        return false;
    // retrieve file size
    uint32_t fileSize = getLengthFromHeader(buffer_, bufferSize_, url);
    if (fileSize <= 1)
        return false;
    uint16_t nParts = (fileSize - 1)/bufferSize_ + 1;
    download_t download = {fileSize, nParts};

    // create the local file
    if (!open(filepath_, O_CREAT | O_WRITE))
        return false;
    // download the file piece by piece
    for (uint32_t i = 0; i < nParts; i++) {
        int nErrors = 0;
        while (nErrors++ < 3) {
            int nBytes = getPart(i, url, download);
            if (nBytes > 0) {
                // write data to SD card
                write(buffer_, nBytes);
                sync();
                break;
            }
            else if (!renewDownloadUrl(url)) {
                sd_->remove(filepath_);
                close();
                return false;
            }
        }
    }
    // close the file
    close();
    return true;
}
//------------------------------------------------------------------------------
/*
 * Check whether downloading from SoundCloud is enabled in the application
 * settings.
 *
 * \return true is returns if downloading is enabled, false if it is disabled.
 */
bool SoundCloud::downloadsEnabled() {
    return (strcmp("1", settings_->getByName(downloadsEnabled_)) == 0);
}
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the path of the file on the SD card.
 *
 * \return the path is returned as a 8.3 filepath.
 */
const char* SoundCloud::filepath() {
    return filepath_;
}
//------------------------------------------------------------------------------
/*
 * Calls the webservice to check if there is a recent track available from the
 * owner that's currently saved in the application settings.
 *
 * \return This method returns true if there is a recent track available and
 * there is no trace of it on the SD card. If there is no recent track, or if
 * there is one but it has already been downloaded, false is returned.
 */
bool SoundCloud::initializeDownload() {
  // perform an API call to retrieve the download link
    if (!api_->connect())
        return false;
    if (api_->call(SOUNDCLOUD_API_METHOD, SOUNDCLOUD_OWNER_KEY,
        settings_->getByName(owner_)) <= 0) {
        return false;
    }
    // parse track name
    char filename[16] = {0};
    if (api_->getStringByName_P(SOUNDCLOUD_NAME_KEY, filename, 16) <= 0)
        return false;
    // construct local path
    snprintf(filepath_, 32, "%S/%s", SOUNDCLOUD_DIRECTORY, filename);
    // initialize SD card
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    // if the file already exists, cancel the download
    if (sd_->exists(filepath_)) {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
/*
 * Determine the size of the resource located at the given URL.
 *
 * \param[out] buffer The buffer where the response will be written.
 * \param[in] bufferSize The size of the output buffer.
 * \param[in] host The remote host where the resource is located.
 * \param[in] path The path of the desired resource on the host.
 *
 * \return The size of the file (in bytes) is returned. If an error occurs, the
 * value 0 is returned.
 *
 * \note This method is a workaround for determining the size of a file on the
 * SoundCloud servers.
 *
 * SoundCloud songs are hosted either on ec-media.soundcloud.com or on
 * ak-media.soundcloud.com, and the latter does not support HEAD requests.
 *
 * In order to retrieve a Content-Length header from ak-media.soundcloud.com,
 * we send a partial GET request with a very small range (only a few bytes) so
 * that we get the response headers.
 */
uint32_t SoundCloud::getLengthFromHeader(char* buffer, size_t bufferSize,
    url_t& url) {
    // create a partial GET request asking for only two bytes
    uint8_t flags = (F_GET | F_KEEP_ALIVE);
    if (!createGetRequest(buffer, bufferSize, url.host, url.path, flags, 0, 1))
        return false;
    wifly_->clear();
    if (!wifly_->print(buffer))
        return 0;
    memset(buffer, 0x00, bufferSize);
    if (!wifly_->awaitResponse())
        return 0;
    // parse the size of the file from the Content-Range header
    if (!wifly_->findUntil_P(HTTP_CONTENT_RANGE, HTTP_END_OF_HEADER))
        return 0;
    memset(buffer, 0x00, 33);
    uint8_t nBytes = wifly_->readBytesUntil_P(HTTP_CRLF, buffer, 32);
    return atol(buffer);
}
//------------------------------------------------------------------------------
/*
 * Download a single part of a file to the buffer.
 *
 * \param[in] partNo The number of the part to download.
 * \param[in] host The host where the file is located.
 * \param[in] path The path to the file on the host.
 * \param[in] fileSize The size of the file.
 * \param[in] nParts The total number of parts in the file.
 *
 * \return The number of bytes downloaded is returned. The value -1 is returned
 * if an error occurs.
 */
int SoundCloud::getPart(uint32_t partNo, url_t& url, download_t &download) {
    // compute the position of the part within the file
    uint32_t firstByte, lastByte;
    firstByte = partNo * bufferSize_;
    if (partNo < download.nParts - 1) {
        lastByte = firstByte + bufferSize_ - 1;
    }
    else if (partNo == download.nParts - 1) {
        lastByte = download.size - 1;
    }
    // download the part to the SRAM buffer
    uint8_t nAttempts = 0;
    do {
        if (getRange(buffer_, bufferSize_, url.host, url.path, firstByte,
            lastByte)) {
            Serial.println(download.size - lastByte);
            return (lastByte - firstByte + 1);
        }
    } while (++nAttempts < 3);
    return -1;
}
//------------------------------------------------------------------------------
/*
 * Make a call to the webservice to retrieve a new URL for the current download.
 * This is necessary because the links generated to access streams on SoundCloud
 * are only valid for 300 seconds.
 *
 * \return true is returned if a new link is successfully obtained, false is
 * returned if an error occurs.
 */
bool SoundCloud::renewDownloadUrl(url_t& url) {
     // clear path buffer
    memset(url.path, 0x00, PATH_BUFFER_SIZE);
    // fetch a new link to the file
    if (!api_->connect()) {
        return false;
    }
    api_->call(SOUNDCLOUD_API_METHOD, SOUNDCLOUD_OWNER_KEY,
        settings_->getByName(owner_));
    if (api_->getStringByName_P(SOUNDCLOUD_URL_KEY, url.path,
        PATH_BUFFER_SIZE) <= 0) {
        return false;
    }
    Serial.println(url.path); // dev only
    return (connect(url.host));
}
