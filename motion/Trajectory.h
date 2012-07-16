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
#ifndef TRAJECTORY_H
#define TRAJECTORY_H
/**
 * \file
 * \brief Trajectory interface.
 */
//------------------------------------------------------------------------------
/**
 * \class Trajectory
 * \brief Interface for trajectory planners with various input sources.
 */
class Trajectory {
public:
    /** Start processing trajectory data from a given source */
    virtual bool begin(const char* source) = 0;
    /** Stop processing trajectory data */
    virtual void end() = 0;
    /** Get the next part of the trajectory */
    virtual bool getNextMotion(int* dest, int* dur, uint8_t pos) = 0;
    /** Compute the next elementary step of the trajectory */
    virtual bool getNextStep(int* step, int* dur, int pos, int dest,
        int timeLeft) = 0;
};

#endif // TRAJECTORY_H
