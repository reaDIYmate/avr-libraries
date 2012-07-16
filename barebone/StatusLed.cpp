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
 #include <StatusLed.h>
//------------------------------------------------------------------------------
/** Construct an instance of the Button FSM */
StatusLed::StatusLed(uint8_t greenLedPin, uint8_t redLedPin) :
    greenLedPin_(greenLedPin),
    redLedPin_(redLedPin)
{
}
//------------------------------------------------------------------------------
/** Turn off the status LED */
void StatusLed::colorNothing() {
    digitalWriteFast(greenLedPin_, LOW);
    digitalWriteFast(redLedPin_, LOW);
}
//------------------------------------------------------------------------------
/** Set the status LED color to green */
void StatusLed::colorGreen() {
    digitalWriteFast(greenLedPin_, HIGH);
    digitalWriteFast(redLedPin_, LOW);
}
//------------------------------------------------------------------------------
/** Set the status LED color to orange */
void StatusLed::colorOrange() {
    digitalWriteFast(greenLedPin_, HIGH);
    digitalWriteFast(redLedPin_, HIGH);
}
//------------------------------------------------------------------------------
/** Set the status LED color to green */
void StatusLed::colorRed() {
    digitalWriteFast(greenLedPin_, LOW);
    digitalWriteFast(redLedPin_, HIGH);
}
//------------------------------------------------------------------------------
/** Configure pins */
void StatusLed::initialize() {
    pinMode(greenLedPin_, OUTPUT);
    pinMode(redLedPin_, OUTPUT);
    colorNothing();
}
