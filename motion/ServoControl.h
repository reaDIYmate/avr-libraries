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
#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H
/**
 * \file
 * \brief ServoControl class to control a servo along a planned trajectory.
 */
#include <Arduino.h>
#include <Servo.h>
#include <Trajectory.h>
//------------------------------------------------------------------------------
class ServoControl : public Servo {
/**
 * \class ServoControl
 * \brief Synchronize with an input trajectory step by step.
 */
public:
    /**
     * \struct Point
     * \brief Store a position of the output on the angle/time plane.
     */
    struct Point {
        /** Target position */
        int target;
        /** Target deadline */
        uint32_t deadline;
    };
    ServoControl(uint8_t servoPin);
    ServoControl(Trajectory &trajectory, uint8_t servoPin);
    /** Return the target position of the current motion */
    int currentTarget() {return motion_.target;}
    bool begin(const char* source = NULL);
    bool begin(Trajectory* trajectory, const char* source = NULL);
    void end();
    bool finishedMotion();
    bool finishedStep();
    bool startNextMotion(uint32_t timestamp = 0);
    bool startNextStep();
//------------------------------------------------------------------------------
private:
    /** Pin used to send pulses to the servo */
    uint8_t servoPin_;
    /** Target of the next motion */
    Point motion_;
    /** Target of the next step in the current motion */
    Point step_;
    /** Trajectory object to use for motion planning */
    Trajectory *trajectory_;
};

#endif // SERVOCONTROL_H
