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
#include <Player.h>
//------------------------------------------------------------------------------
/** Construct an instance of the Player FSM */
Player::Player(Audio &audio) :
    audio_(&audio)
{
    internalTransition(Player::stopped);
    dispatch(entryEvt);
}
//------------------------------------------------------------------------------
/** The player is completely stopped */
void Player::stopped(const Event* e) {
    switch (e->signal) {
        case PLAY :
            if (audio_->open(((PlayerEvent*)e)->filename))
                transition(Player::playing);
            else
                emit(END_OF_FILE);
            break;
        case RANDOM :
            if (audio_->openRandom(((PlayerEvent*)e)->filename,
                ((PlayerEvent*)e)->filecount))
                transition(Player::playing);
            else
                emit(END_OF_FILE);
            break;
    }
}
//------------------------------------------------------------------------------
/** The player is playing a file */
void Player::playing(const Event* e) {
    switch (e->signal) {
        case TICK :
            if (!audio_->playNextFrame()) {
                audio_->stopPlaying();
                emit(END_OF_FILE);
                transition(Player::stopped);
            }
            break;
        case PAUSE :
            transition(Player::paused);
            break;
        case PLAY :
            audio_->stopPlaying();
            if (audio_->open(((PlayerEvent*)e)->filename))
                transition(Player::playing);
            else
                transition(Player::stopped);
            break;
        case RANDOM :
            audio_->stopPlaying();
            if (audio_->openRandom(((PlayerEvent*)e)->filename,
                ((PlayerEvent*)e)->filecount))
                transition(Player::playing);
            else
                transition(Player::stopped);
            break;
        case STOP :
            audio_->stopPlaying();
            transition(Player::stopped);
            break;
    }
}
//------------------------------------------------------------------------------
/** The player is paused */
void Player::paused(const Event* e) {
    switch (e->signal) {
        case PLAY :
            transition(Player::playing);
            break;
        case STOP :
            transition(Player::stopped);
            break;
    }
}
