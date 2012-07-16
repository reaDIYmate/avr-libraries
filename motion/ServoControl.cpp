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
#include <ServoControl.h>
//------------------------------------------------------------------------------
/**
 * Construct an instance of ServoControl.
 *
 * \param[in] trajectory The input trajectory.
 * \param[in] servoPin The pin commanding the output servo.
 */
ServoControl::ServoControl(Trajectory &trajectory, uint8_t servoPin) :
    trajectory_(&trajectory),
    servoPin_(servoPin)
{
}
//------------------------------------------------------------------------------
/**
 * Construct an instance of ServoControl.
 *
 * \param[in] servoPin The pin commanding the output servo.
 */
ServoControl::ServoControl(uint8_t servoPin) : servoPin_(servoPin) {}
//------------------------------------------------------------------------------
/**
 * Start following the input trajectory.
 *
 * \param[in] source A string parameter passed to the trajectory.
 *
 * \return true is returned if the trajectory is successfully started.
 */
bool ServoControl::begin(const char* source) {
    if (trajectory_ == NULL)
        return false;
    attach(servoPin_);
    // consider instant 0 to be right before opening the file
    motion_.deadline = millis();
    return (trajectory_->begin(source));
}
//------------------------------------------------------------------------------
/**
 * Start following a trajectory.
 *
 * \param[in] trajectory The input trajectory.
 * \param[in] source A string parameter passed to the trajectory.
 *
 * \return true is returned if the trajectory is successfully started.
 */
bool ServoControl::begin(Trajectory* trajectory, const char* source) {
    trajectory_ = trajectory;
    return begin(source);
}
//------------------------------------------------------------------------------
/** Stop moving */
void ServoControl::end() {
    if (trajectory_ == NULL)
        return;
    trajectory_->end();
    detach();
}
//------------------------------------------------------------------------------
/**
 * Check whether the deadline for the current motion has been reached.
 *
 * \return true is returned if it is time to start the next motion.
 */
bool ServoControl::finishedMotion() {
    return (millis() >= motion_.deadline);
}
//------------------------------------------------------------------------------
/**
 * Check whether the deadline for the current step has been reached.
 *
 * \return true is returned if it is time to start the next step.
 */
bool ServoControl::finishedStep() {
    return (millis() >= step_.deadline);
}
//------------------------------------------------------------------------------
/**
 * Start the next motion.
 *
 * \param[in] timestamp The value to consider the starting point of the motion.
 *
 * \return true is returned if the next motion is successfully started.
 */
bool ServoControl::startNextMotion(uint32_t timestamp) {
    int destination;
    int duration;
    if (trajectory_->getNextMotion(&destination, &duration, read())) {
        if (timestamp != 0)
            motion_.deadline = timestamp;
        motion_.deadline += duration;
        motion_.target = destination;
        return true;
    }
    else {
        return false;
    }
}
//------------------------------------------------------------------------------
/**
 * Take the next step.
 *
 * \return true is returned if the next step is successfully taken.
 */
bool ServoControl::startNextStep() {
    int step;
    int duration;
    int timeLeft = (int)(motion_.deadline - millis());
    if (trajectory_->getNextStep(&step, &duration, read(), motion_.target, timeLeft)) {
        step_.deadline = millis() + duration;
        step_.target = step;
        write(read() + step_.target);
        return true;
    }
    else {
        return false;
    }
}
