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
#ifndef FSM_H
#define FSM_H
/**
 * \file
 * \brief Fsm class to run finite state machines.
 */
#include <Signals.h>
//------------------------------------------------------------------------------
/**
 * Call transition_ after casting a subclass state to the right pointer-to-
 * member-function type before.
 */
#define transition(state) transition_((Fsm::State)&state)
/**
 * Call internalTransition_ after casting a subclass state to the right pointer-
 * to-member-function type before.
 */
#define internalTransition(state) internalTransition_((Fsm::State)&state)
//------------------------------------------------------------------------------
/**
 * \struct Event
 * \brief Represents an event that can be passed to a FSM.
 */
struct Event {
    /** Defines the event type */
    Signal signal;
    /** Create a new empty event */
    Event() : signal(NOTHING) {}
    /** Create a new event */
    Event(Signal sig) : signal(sig) {}
};
//------------------------------------------------------------------------------
class Fsm {
/**
 * \class Fsm
 * \brief An FSM class with states represented as pointers to member functions.
 */
public:
    void dispatch(const Event &eIn);
    void dispatch(const Event &eIn, Event &eOut);
    /** Pointer to member function type used to store state handlers */
    typedef void (Fsm::*State)(Event const *);
//------------------------------------------------------------------------------
protected:
    /** Set the output signal */
    void emit(Signal signal) {out_ = signal;}
    void internalTransition_(State newState);
    void transition_(State newState);
    /** Output signal */
    Signal out_;
    /** Current state */
    State state_;
    /** Preinstantiated entry event */
    static const Event entryEvt;
    /** Preinstantiated exit event */
    static const Event exitEvt;
};

#endif // FSM_H
