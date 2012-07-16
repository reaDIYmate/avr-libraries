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
#ifndef SIGNAL_H
#define SIGNAL_H
enum Signal {
    NOTHING = 0x00,
    ALERT,
    CONTACT_DETECTED,
    END_OF_FILE,
    ENTRY,
    EXIT,
    FALL_ASLEEP,
    INBOX_MESSAGE,
    LONG_CLICK_ARMED,
    LONG_CLICK_RELEASED,
    POKE_NO,
    POKE_RECEIVED,
    POKE_SENT,
    POKE_WARNING,
    POKE_YES,
    MOTION_PERFORMED,
    NO_CONTACT_DETECTED,
    PAUSE,
    PLAY,
    PLAY_ALERT,
    POLL_ALERTS,
    POLL_INBOX,
    RANDOM,
    RANDOM_ACTIVITY,
    SHORT_CLICK_RELEASED,
    STOP,
    SUPERLONG_CLICK_ARMED,
    TICK,
    TILT,
    WAKE_UP
};

#endif // SIGNAL_H
