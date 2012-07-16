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
#include <Motion.h>
//------------------------------------------------------------------------------
/** Construct an instance of the Motion FSM */
Motion::Motion(ServoControl &control, SdFat &sd, uint8_t sdChipSelectPin) :
    control_(&control),
    trajectory_(sd, sdChipSelectPin)
{
    internalTransition(Motion::stopped),
    dispatch(entryEvt);
}
//------------------------------------------------------------------------------
/** Completely immobile */
void Motion::stopped(const Event* e) {
    switch (e->signal) {
        case PLAY :
            if (control_->begin(&trajectory_, ((MotionEvent*)e)->filename))
                transition(Motion::computingNextMotion);
            else
                emit(END_OF_FILE);
            break;
    }
}
//------------------------------------------------------------------------------
/** Computing the next motion */
void Motion::computingNextMotion(const Event* e) {
    switch (e->signal) {
        case TICK :
            if (control_->startNextMotion())
                transition(Motion::computingNextStep);
            else {
                control_->end();
                transition(Motion::stopped);
                emit(END_OF_FILE);
            }
            break;
        case STOP :
            control_->end();
            transition(Motion::stopped);
            break;
    }
}
//------------------------------------------------------------------------------
/** Computing the next step */
void Motion::computingNextStep(const Event* e) {
    switch (e->signal) {
        case TICK :
            control_->startNextStep();
            transition(Motion::waitingForStepEnd);
            break;
        case STOP :
            control_->end();
            transition(Motion::stopped);
            break;
    }
}
//------------------------------------------------------------------------------
/** Awaiting the end of the current step */
void Motion::waitingForStepEnd(const Event* e) {
    switch (e->signal) {
        case TICK :
            if (control_->finishedMotion())
                transition(Motion::computingNextMotion);
            else if (control_->finishedStep())
                transition(Motion::computingNextStep);
            break;
        case STOP :
            control_->end();
            transition(Motion::stopped);
            break;
    }
}
