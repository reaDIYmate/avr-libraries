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
#include <PusherTrajectory.h>
//------------------------------------------------------------------------------
/*
 * x(-i) is the i-th most recent sample
 * since we only use a 1st-order lowpass filter we only actually need
 * x(0) and x(-1).
 */
#define x(i) x[(WINDOW_SIZE + ix_ + i) % WINDOW_SIZE]
#define y(i) y[(WINDOW_SIZE + iy_ + i) % WINDOW_SIZE]
/** Maximum output for the servo (in degrees) */
uint8_t const THETA_MAX = 150;
/** Minimum output for the servo (in degrees) */
uint8_t const THETA_MIN = 0;
/** FIR gain */
const float alpha = 0.65;
//------------------------------------------------------------------------------
/**
 * Construct an instance of PusherTrajectory.
 *
 * \param[in] pusherClient The Pusher client to receive trajectory instructions
 * from.
 * \param[in] key Public key for the Pusher application.
 * \param[in] secret Private key for the Pusher application.
 */
PusherTrajectory::PusherTrajectory(PusherClient &pusherClient) :
    pusher_(&pusherClient)
{
}
//------------------------------------------------------------------------------
/**
 * Start processing incoming events on a given channel.
 *
 * \param[in] channel A Flash-based string representing the name of the channel.
 *
 * \return true is returned if authentication and connection to the channel is
 * successfull.
 */
bool PusherTrajectory::begin(const char* channel) {
    y(0) = THETA_MIN;
    y(-1) = THETA_MIN;

    // connect to Pusher
    if (!pusher_->connected())
        return false;
}
//------------------------------------------------------------------------------
/** Stop processing incoming events */
void PusherTrajectory::end() {
    pusher_->disconnect();
}
//------------------------------------------------------------------------------
/**
 * Update target position.
 *
 * \param[in] input The new input sample.
 *
 * \return The new output value of the filter is returned.
 */
int PusherTrajectory::filter(int input) {
    // update input ring buffer
    ++ix_ %= WINDOW_SIZE;
    x(0) = input;

    // update output ring buffer
    // the filter equation (1st-order lowpass) is:
    // y(i) = alpha * x(i) + (1 - alpha) * y(i - 1)
    ++iy_ %= WINDOW_SIZE;
    y(0) = (int)(float)y(-1) + alpha*((float)(x(0) - y(-1)));
    return y(0);
}
//------------------------------------------------------------------------------
/**
 * Read the next motion from the Pusher channel.
 *
 * \param[out] dest Location where the motion destination will be written.
 * \param[out] dur Location where the motion duration will be written.
 * \param[in] pos Last known position of the output.
 *
 * \return true is returned if a new event was processed. The value false is
 * returned if no event is available.
 */
bool PusherTrajectory::getNextMotion(int* dest, int* dur, uint8_t pos) {
    if (!pusher_->hasNextEvent())
        return false;
    int tilt = pusher_->getIntegerByName("tilt");
    if (tilt == END_OF_TRAJECTORY) {
        *dest = END_OF_TRAJECTORY;
        return true;
    }
    if (tilt < THETA_MIN || tilt > THETA_MAX)
        return false;
    else {
        *dest = filter(tilt);
        *dur = INPUT_SAMPLING_PERIOD;
        return true;
    }
}
//------------------------------------------------------------------------------
/**
 * Compute the next step within the ongoing motion.
 *
 * \param[out] step Location where the step value will be written.
 * \param[out] dur Location where the motion duration will be written.
 * \param[in] pos Last known position of the output.
 * \param[in] dest Target position of the current motion.
 * \param[in] timeLeft Time remaining (in ms) to complete the ongoing motion.
 *
 * \return true is returned if a new step was started.
 */
bool PusherTrajectory::getNextStep(int* step, int* dur, int pos, int dest,
    int timeLeft) {
    // don't move if the target position is reached
    int distanceLeft = dest - pos;
    if (distanceLeft == 0) {
        *step = 0;
        *dur = timeLeft;
        return false;
    }
    else {
        // steps are 1° up or 1° down
        *step = (distanceLeft > 0) ? 1 : -1;
        // compute speed and step duration
        float speed = (float)timeLeft/(float)(distanceLeft);
        *dur = (int)(((float)(*step))*speed);
        // angular speed is capped at 62.5°/s
        if (*dur < 16) *dur = 16;
        return true;
    }
}
