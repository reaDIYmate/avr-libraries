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
#ifndef BUTTON_H
#define BUTTON_H
/**
 * \file
 * \brief Use a button to interact with the object and control its state.
 */
#include <digitalWriteFast.h>
#include <Fsm.h>
#include <StatusLed.h>
//------------------------------------------------------------------------------
class Button : public Fsm {
/**
 * \class Button
 * \brief A state machine that debounces the button and generates click events.
 */
public:
    Button(uint8_t buttonPin, StatusLed &led);
    void initialize();
//------------------------------------------------------------------------------
private:
    // States
    void notPushed(const Event* e);
    void maybePushed(const Event* e);
    void pushedShort(const Event* e);
    void pushedLong(const Event* e);
    void pushedSuperLong(const Event* e);
    /** Pin connected to the pushbutton (active LOW) */
    const uint8_t buttonPin_;
    /** Timestamp used to trigger short click events */
    unsigned long shortClickDeadline;
    /** Timestamp used to trigger long click events */
    unsigned long longClickDeadline;
    /** Timestamp used to trigger superlong click events */
    unsigned long superLongClickDeadline;
    /** Status LED */
    StatusLed* led_;
};

#endif // BUTTON_H
