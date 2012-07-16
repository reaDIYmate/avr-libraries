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
#include <Fsm.h>
//------------------------------------------------------------------------------
/**
 * Process and incoming event.
 *
 * \param[in] eIn The event to handle.
 * \param[out] eOut The event to use for output.
 */
void Fsm::dispatch(const Event &eIn, Event &eOut) {
    emit(NOTHING);
    (*this.*state_)(&eIn);
    eOut.signal = out_;
}
//------------------------------------------------------------------------------
/**
 * Process and incoming event.
 *
 * \param[in] eIn The event to handle.
 */
void Fsm::dispatch(const Event &eIn) {
    emit(NOTHING);
    (*this.*state_)(&eIn);
}
//------------------------------------------------------------------------------
/**
 * Perform an internal transition (no entry/exit actions).
 *
 * \param[in] newState New state after the transition.
 */
void Fsm::internalTransition_(State newState) {
    state_ = newState;
}
//------------------------------------------------------------------------------
/**
 * Perform a state transition.
 *
 * \param[in] newState New state after the transition.
 */
void Fsm::transition_(State newState) {
    (*this.*state_)(&exitEvt);
    state_ = newState;
    (*this.*state_)(&entryEvt);
}
//------------------------------------------------------------------------------
/** Entry event */
const Event Fsm::entryEvt(ENTRY);
/** Exit event */
const Event Fsm::exitEvt(EXIT);
