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
 #include <Button.h>
//------------------------------------------------------------------------------
const long SHORT_CLICK_DEBOUNCE = 100;
const long LONG_CLICK_DEBOUNCE = 1000;
const long SUPERLONG_CLICK_DEBOUNCE = 2000;
//------------------------------------------------------------------------------
/** Construct an instance of the Button FSM */
Button::Button(uint8_t buttonPin) :
    buttonPin_(buttonPin)
{
    internalTransition(Button::notPushed);
    dispatch(entryEvt);
}
//------------------------------------------------------------------------------
/** Initialize the button. */
void Button::initialize() {
    pinMode(buttonPin_, INPUT);
    // enable pullup resistor
    digitalWrite(buttonPin_, HIGH);
}
//------------------------------------------------------------------------------
/** No contact detected. */
void Button::notPushed(const Event* e) {
    switch (e->signal) {
        case CONTACT_DETECTED :
            transition(Button::maybePushed);
            break;
    }
}
//------------------------------------------------------------------------------
/**
 * Contact has been detected at least once but not long enouh to produce a
 * short click.
 */
void Button::maybePushed(const Event* e) {
    switch (e->signal) {
        case ENTRY :
            shortClickDeadline = millis() + SHORT_CLICK_DEBOUNCE;
            break;
        case NO_CONTACT_DETECTED :
            transition(Button::notPushed);
            break;
        case CONTACT_DETECTED :
            if (millis() >= shortClickDeadline) {
                transition(Button::pushedShort);
            }
            break;
    }
}
//------------------------------------------------------------------------------
/** Button pushed long enough to produce at least a short click. */
void Button::pushedShort(const Event* e) {
    switch (e->signal) {
        case ENTRY :
            longClickDeadline = millis() + LONG_CLICK_DEBOUNCE;
            break;
        case NO_CONTACT_DETECTED :
            emit(SHORT_CLICK_RELEASED);
            transition(Button::notPushed);
            break;
        case CONTACT_DETECTED :
            if (millis() >= longClickDeadline) {
                emit(LONG_CLICK_ARMED);
                transition(Button::pushedLong);
            }
            break;
    }
}
//------------------------------------------------------------------------------
/** Button pushed long enough to produce at least a long click. */
void Button::pushedLong(const Event* e) {
    switch(e->signal) {
        case ENTRY :
            superLongClickDeadline = millis() + SUPERLONG_CLICK_DEBOUNCE;
            break;
        case NO_CONTACT_DETECTED :
            emit(LONG_CLICK_RELEASED);
            transition(Button::notPushed);
            break;
        case CONTACT_DETECTED :
            if (millis() >= superLongClickDeadline) {
                emit(SUPERLONG_CLICK_ARMED);
                transition(Button::pushedSuperLong);
            }
            break;
    }
}
//------------------------------------------------------------------------------
/** Button pushed long enough to produce at least superlong click. */
void Button::pushedSuperLong(const Event* e) {
    switch(e->signal) {
        case NO_CONTACT_DETECTED :
            transition(Button::notPushed);
            break;
    }
}
