/* reaDIYmate library
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
#ifndef STATUS_LED_H
#define STATUS_LED_H
/**
 * \file
 * \brief StatusLed class for status LED color control.
 */
#include <digitalWriteFast.h>
//------------------------------------------------------------------------------
/**
 * \class StatusLed
 * \brief The status LED is a dual-color Red/Green LED.
 */
class StatusLed {
public:
    StatusLed(uint8_t greenLedPin, uint8_t redLedPin);
    void initialize();
    void colorNothing();
    void colorGreen();
    void colorOrange();
    void colorRed();
//------------------------------------------------------------------------------
private:
    /** Pin for the green LED (active HIGH) */
    const uint8_t greenLedPin_;
    /** Pin for the red LED (active HIGH) */
    const uint8_t redLedPin_;
};

#endif // STATUS_LED_H
