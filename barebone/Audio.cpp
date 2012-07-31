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
#include <Audio.h>
//------------------------------------------------------------------------------
/**
 * Construct an instance of Audio.
 *
 * \param[in] dataRequestPin The AVR pin connected to DREQ on the VS1011.
 * \param[in] resetPin The AVR pin connected to RESET on the VS1011.
 * \param[in] dataSelectPin The AVR pin connected to XDCS/BSYNC on the VS1011.
 * \param[in] controlSelectPin The AVR pin connected to XCS on the VS1011.
 * \param[in] sd A SdFat object
 * \param[in] sdChipSelectPin The AVR pin connected to CS on the SD card.
 */
Audio::Audio(uint8_t dataRequestPin, uint8_t resetPin, uint8_t dataSelectPin,
    uint8_t controlSelectPin, SdFat &sd, uint8_t sdChipSelectPin) :
    Vs1011(dataRequestPin, resetPin, dataSelectPin, controlSelectPin),
    sd_(&sd),
    sdChipSelectPin_(sdChipSelectPin)
{
}
//------------------------------------------------------------------------------
/**
 * Initialize the MP3 decoder and the SD card, then open the file.
 *
 * \param[in] filename The file to open.
 *
 * \return true is returned for success and false is returned for failure.
 */
//-----------------------------------------------------------------------------
bool Audio::open(const char* filename) {
    initialize();
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_))
        return false;
    return SdFile::open(filename, O_READ);
}
//------------------------------------------------------------------------------
/**
 * Initialize the MP3 decoder and the SD card, then open a random file in the
 * indicated directory.
 *
 * \param[in] directory The directory to open.
 * \param[in] count Number of files in the directory.
 *
 * \return true is returned for success and false is returned for failure.
 */
//-----------------------------------------------------------------------------
bool Audio::openRandom(const char* directory, uint16_t count) {
    initialize();
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_))
        return false;
    if (!sd_->chdir(directory, true))
        return false;
    rewind();

    uint16_t nbEntries = sd_->vwd()->fileSize()/32;
    uint16_t index = 0;
    // count files
    if (count == 0) {
        while (index < nbEntries) {
            if (!SdFile::open(sd_->vwd(), ++index, O_READ))
                continue;
            if (!isFile())
                continue;
            ++count;
            SdFile::close();
        }
    }
    // skip a random number of files
    uint16_t target = random(count);
    uint16_t nbFiles = 0;
    index = 0;
    while (nbFiles < target + 1 && index < nbEntries) {
        if (!SdFile::open(sd_->vwd(), ++index, O_READ))
            continue;
        if (!isFile())
            continue;
        ++nbFiles;
        SdFile::close();
    }
    if (!SdFile::open(sd_->vwd(), index, O_READ))
        return false;
    sd_->chdir(true);
    return true;
}
//------------------------------------------------------------------------------
/**
 * Read one frame from the file and send it to the decoder.
 *
 * \return true is returned for success and false is returned for failure.
 */
bool Audio::playNextFrame() {
    uint16_t nBytes;
    nBytes = read(buffer_, sizeof(buffer_));
    if (nBytes > 0) {
        sendBytes(buffer_, sizeof(buffer_));
    }
    return (nBytes == sizeof(buffer_));
}
//------------------------------------------------------------------------------
/**
 * Play one audio file from the beginning to the end.
 *
 * \return true is returned for success and false is returned for failure.
 */
bool Audio::play(const char* filename) {
    if (!open(filename))
        return false;
    while (playNextFrame());
    stopPlaying();
    return true;
}
//------------------------------------------------------------------------------
/**
 * End audio playback
 *
 * \note In order to make sure the VS1011 doesn't cut the ending of low-bitrate
 * data streams when forcing a software reset, the VS1011 documentation
 * recommends to feed 2048 zeros to the SDI bus after the file and before the
 * reset.
 */
void Audio::stopPlaying() {
    sendZeros(2048);
    close();
}
