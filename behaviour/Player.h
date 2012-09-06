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
#ifndef PLAYER_H
#define PLAYER_H
/**
 * \file
 * \brief Manage a stream of MP3 data from a file to the DSP.
 */
#include <Audio.h>
#include <Fsm.h>
//------------------------------------------------------------------------------
/**
 * \struct PlayerEvent
 * \brief %Event generated to send command to the %Player FSM.
 */
struct PlayerEvent : public Event {
    /** Name of the audio file targeted by the event */
    const char* filename;
    /** Create a new PlayerEvent targeting a file or a directory */
    PlayerEvent(Signal sig, const char* str) : Event(sig), filename(str) {}
};
//------------------------------------------------------------------------------
class Player : public Fsm {
/**
 * \class Player
 * \brief A state machine that controls the audio flow.
 */
public:
    Player(Audio &audio);
//------------------------------------------------------------------------------
private:
    // States
    void stopped(const Event* e);
    void playing(const Event* e);
    void paused(const Event* e);
    /** Audio file handler */
    Audio* audio_;
};

#endif // PLAYER_H
