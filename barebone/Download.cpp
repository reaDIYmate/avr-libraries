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
#include <Download.h>
//------------------------------------------------------------------------------
/** Size of the temporary buffer used to read file host */
uint8_t const DOWNLOAD_HOST_BUFFER_SIZE = 128;
//------------------------------------------------------------------------------
/**
 * Construct an instance of Download.
 *
 * \param[in] wifly The Wifly object to use for communications
 * \param[in] buffer The Buffer to use for downloading.
 * \param[in] bufferSize The size of the buffer.
 * \param[in] host The remote host where the file is located.
 * \param[in] sd A SdFat object
 * \param[in] sdChipSelectPin The AVR pin connected to CS on the SD card.
 */
Download::Download(Wifly &wifly, char* buffer, size_t bufferSize,
    PGM_P host, SdFat &sd, uint8_t sdChipSelectPin) :
    HttpClient(wifly),
    buffer_(buffer),
    bufferSize_(bufferSize),
    host_(host),
    sd_(&sd),
    sdChipSelectPin_(sdChipSelectPin)
{
}
//------------------------------------------------------------------------------
/**
 * Open a connection to the host.
 *
 * \return true is returned if a connection is successfully opened to the host.
 * Otherwise, false is returned.
 */
bool Download::connect() {
    char host[WIFLY_HOST_BUFFER_SIZE] = {0};
    strlcpy_P(host, host_, WIFLY_HOST_BUFFER_SIZE);
    return HttpClient::connect(host);
}
//------------------------------------------------------------------------------
/** Check whether the client is connected to the host */
bool Download::connected() {
    return wifly_->connectedTo_P(host_);
}
//------------------------------------------------------------------------------
/**
 * Save the file located at the given path to the SD card with the provided
 * local name.
 *
 * \param[in] path The path to the file on the host.
 * \param[in] localName The desired name for the local copy of the file.
 *
 * \return true if the download is successfull, false in case of failure.
 */
bool Download::save(const char* path, const char* localName) {
    // check socket
    if (!wifly_->connectedTo_P(host_))
        return false;

    // load host name to SRAM
    char host[DOWNLOAD_HOST_BUFFER_SIZE];
    strlcpy_P(host, host_, DOWNLOAD_HOST_BUFFER_SIZE);

    // fetch content length and compute the number of pieces
    uint32_t fileSize = getContentLength(buffer_, bufferSize_, host, path);
    if (fileSize == 0)
        return false;
    uint16_t nbPieces = (fileSize - 1)/bufferSize_ + 1;

    // create the local file
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    if (exists(localName))
        remove(this, localName);
    if (!open(localName, O_CREAT | O_WRITE)) {
        return false;
    }

    // download the file piece by piece
    uint32_t firstByte, lastByte;
    for (uint32_t i = 0; i < nbPieces; i++) {
        firstByte = i * bufferSize_;
        if (i < nbPieces - 1)
            lastByte = firstByte + bufferSize_ - 1;
        else if (i == nbPieces - 1)
            lastByte = fileSize - 1;
        while (!getRange(buffer_, bufferSize_, host, path, firstByte, lastByte));
        write(buffer_, lastByte - firstByte + 1);
        sync();
    }

    // close the file
    close();

    return true;
}
