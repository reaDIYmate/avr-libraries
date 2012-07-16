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
#ifndef AUDIO_H
#define AUDIO_H
/**
 * \file
 * \brief Browse and play audio files.
 */
#include <SdFat.h>
#include <Vs1011.h>
//------------------------------------------------------------------------------
/**
 * \class Audio
 * \brief Browse audio files on a SD card and play them with a VS1011 decoder.
 */
class Audio : public Vs1011, public SdFile {
public:
    Audio(uint8_t dataRequestPin, uint8_t resetPin, uint8_t dataSelectPin,
        uint8_t controlSelectPin, SdFat &sd, uint8_t sdChipSelectPin);
    bool open(const char* filename);
    bool openRandom(const char* directory, uint16_t count = 0);
    bool playNextFrame();
    bool play(const char* filename);
    void stopPlaying();
//------------------------------------------------------------------------------
private:
    /** SD card Chip Select pin */
    const uint8_t sdChipSelectPin_;
    /** Audio frame buffer (holds 32 samples) */
    uint8_t buffer_[32];
    /** An SdFat object to manage the SD card */
    SdFat* sd_;
};

#endif // AUDIO_H
