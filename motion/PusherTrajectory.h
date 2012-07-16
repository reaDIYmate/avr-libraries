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
#ifndef PUSHER_TRAJECTORY_H
#define PUSHER_TRAJECTORY_H
/**
 * \file
 * \brief PusherTrajectory class for realtime trajectory input via a websocket.
 */
 #include <PusherClient.h>
 #include <Trajectory.h>
//------------------------------------------------------------------------------
/** Width of the impulse response */
#define WINDOW_SIZE 2
/** Time interval between two input samples */
#define INPUT_SAMPLING_PERIOD 300
//------------------------------------------------------------------------------
/**
 * \class PusherTrajectory
 * \brief Plan an output trajectory using data from a Pusher websocket.
 */
class PusherTrajectory : public Trajectory {
public:
    PusherTrajectory(PusherClient &pusherClient);
    virtual bool begin(const char* channel);
    virtual void end();
    virtual bool getNextMotion(int* dest, int* dur, uint8_t pos);
    virtual bool getNextStep(int* step, int* dur, int pos, int dest,
        int timeLeft);
//------------------------------------------------------------------------------
private:
    int filter(int input);
    int x[WINDOW_SIZE];
    int y[WINDOW_SIZE];
    int ix_;
    int iy_;
    /** Pusher client used to receive trajectory instructions */
    PusherClient* pusher_;
};

#endif // PUSHER_TRAJECTORY_H
