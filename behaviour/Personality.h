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
#ifndef PERSONALITY_H
#define PERSONALITY_H
/**
 * \file
 * \brief Personality class.
 */
#include <Fsm.h>
#include <Inbox.h>
#include <ServoControl.h>
#include <StatusLed.h>
#include <PusherTrajectory.h>
//------------------------------------------------------------------------------
/**
 * \struct InboxEvent
 * \brief %Event generated when a message is retrieved from the inbox.
 */
struct InboxEvent : public Event {
    /** Message type */
    int messageType;
    /** Create a new InboxEvent */
    InboxEvent(Signal sig, int t) : Event(sig), messageType(t) {}
};
//------------------------------------------------------------------------------
class Personality : public Fsm {
/**
 * \class Personality
 * \brief State machine for interaction management.
 */
public:
    Personality(Api &api, StatusLed &led, Inbox &inbox, ServoControl &control,
        PusherTrajectory &realtime);
    void initialize();
//------------------------------------------------------------------------------
private:
    // States
    void action(const Event* e);
    void asleep(const Event* e);
    void awake(const Event* e);
    void checkingServices(const Event* e);
    void checkingFacebook(const Event* e);
    void checkingFoursquare(const Event* e);
    void checkingGmail(const Event* e);
    void checkingRss(const Event* e);
    void checkingTwitter(const Event* e);
    void checkingSoundCloud(const Event* e);
    void playingSoundCloud(const Event* e);
    void enteringPushMode(const Event* e);
    void fallingAsleep(const Event* e);
    void remoteControl(const Event* e);
    void wakingUp(const Event* e);
    // Helper methods
    void postActivity(uint8_t level);
    void resetDeadlines();
    /** Timestamp used to start checking Gmail*/
    unsigned long checkingGmailDeadline_;
    /** Timestamp used to start checking Facebook*/
    unsigned long checkingFacebookDeadline_;
    /** Timestamp used to start checking Twitter*/
    unsigned long checkingTwitterDeadline_;
    /** Timestamp used to start checking Rss*/
    unsigned long checkingRssDeadline_;
    /** Timestamp used to start checking Foursquare*/
    unsigned long checkingFoursquareDeadline_;
    /** Timestamp used to start checking SoundCloud*/
    unsigned long checkingSoundCloudDeadline_;
    /** Timestamp used to leave remote control mode */
    unsigned long remoteControlDeadline_;
    /** Instance of Api used to access the reaDIYmate API */
    Api* api_;
    /** Instance of StatusLed*/
    StatusLed* led_;
    /** Output controller */
    ServoControl* control_;
    /** Input handler */
    PusherTrajectory* realtime_;
    /* Inbox manager */
    Inbox* inbox_;
};

#endif // PERSONALITY_H
