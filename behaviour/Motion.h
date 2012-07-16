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
#ifndef MOTION_H
#define MOTION_H
/**
 * \file
 * \brief Make a servo follow a trajectory.
 */
#include <Fsm.h>
#include <SdTrajectory.h>
#include <ServoControl.h>
//------------------------------------------------------------------------------
 /**
 * \struct MotionEvent
 * \brief %Event generated to send command to the %Motion FSM.
 */
struct MotionEvent : public Event {
    /** Name of the choregraphy file targeted by the event */
    const char* filename;
    /** Create a new MotionEvent */
    MotionEvent(Signal sig, const char* str) : Event(sig), filename(str) {}
};
//------------------------------------------------------------------------------
class Motion : public Fsm {
/**
 * \class Motion
 * \brief State machine for motion planning and control.
 */
public:
    Motion(ServoControl &control, SdFat &sd, uint8_t sdChipSelectPin);
//------------------------------------------------------------------------------
private:
    // States
    void stopped(const Event* e);
    void computingNextMotion(const Event* e);
    void computingNextStep(const Event* e);
    void waitingForStepEnd(const Event* e);
    /** Output controller */
    ServoControl* control_;
    /** Input handler */
    SdTrajectory trajectory_;
};

#endif // MOTION_H
