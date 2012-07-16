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
#ifndef SD_TRAJECTORY_H
#define SD_TRAJECTORY_H
/**
 * \file
 * \brief SdTrajectory class to parse and plan motion from a file.
 */
#include <SdFat.h>
#include <Trajectory.h>
//------------------------------------------------------------------------------
/**
 * \class SdTrajectory
 * \brief Parse an animation file motion by motion, then decompose each motion
 * in a sequence of steps.
 */
class SdTrajectory : public Trajectory, public SdFile {
public:
    SdTrajectory(SdFat &sd, uint8_t sdChipSelectPin);
    virtual bool begin(const char* filename);
    virtual void end();
    virtual bool getNextMotion(int* dest, int* dur, uint8_t pos);
    virtual bool getNextStep(int* step, int* dur, int pos, int dest,
        int timeLeft);
//------------------------------------------------------------------------------
private:
    bool parseMotion(int* dest, int* dur, const char* data, uint8_t pos);
    int readLine(char* buffer, size_t length);
    /** Line buffer */
    char buffer_[128];
    /** SD card Chip Select pin */
    const uint8_t sdChipSelectPin_;
    /** An SdFat object to manage the SD card */
    SdFat* sd_;
};

#endif // SD_TRAJECTORY_H
