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
#include <Personality.h>
#define DEBUG
//------------------------------------------------------------------------------
/** Name of the API method used to update the thing activity level. */
const char PROGMEM METHOD_POST_ACTIVITY[] = "thing/activity";
/** Key corresponding to the activity level in the API call. */
const char PROGMEM KEY_ACTIVITY[] = "activity";
//------------------------------------------------------------------------------
// Timing constants
/** Refresh interval (in ms) for the inbox messages. */
const uint32_t POLL_INTERVAL = 480000;
/** Refresh interval (in ms) to check Gmail. */
const uint32_t CHECK_GMAIL_INTERVAL = 480000;
/** Refresh interval (in ms) to check Facebook. */
const uint32_t CHECK_FACEBOOK_INTERVAL = 480000;
/** Refresh interval (in ms) to check Twitter. */
const uint32_t CHECK_TWITTER_INTERVAL = 480000;
/** Refresh interval (in ms) to check Rss. */
const uint32_t CHECK_RSS_INTERVAL = 480000;
/** Refresh interval (in ms) to check Foursquare. */
const uint32_t CHECK_FOURSQUARE_INTERVAL = 480000;
/** Refresh interval (in ms) to check SoundCloud. */
const uint32_t CHECK_SOUNDCLOUD_INTERVAL = 3600000;
/** Timeout (in ms) for remote control mode */
const uint32_t REMOTE_CONTROL_TIMEOUT = 30000;
const uint8_t MAX_LEVEL = 1;
//------------------------------------------------------------------------------
/** Construct an instance of the Personality FSM */
Personality::Personality(Api &api, Inbox &inbox, ServoControl &control,
    PusherTrajectory &realtime) :
    api_(&api),
    inbox_(&inbox),
    control_(&control),
    realtime_(&realtime)
{
    internalTransition(Personality::asleep);
}
//------------------------------------------------------------------------------
void Personality::initialize() {
    postActivity(MAX_LEVEL);
    transition(Personality::awake);
}
//------------------------------------------------------------------------------
/** Action */
void Personality::action(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::action"));
#endif
            break;
        case STOP :
            internalTransition(Personality::awake);
            Serial.println(F("Personality::awake"));
            break;
        case SOUNDCLOUD :
            transition(Personality::playingSoundCloud);
            break;
    }
}
//------------------------------------------------------------------------------
/** Asleep */
void Personality::asleep(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::asleep"));
#endif
            pushModeDeadline_ = millis() + REMOTE_CONTROL_TIMEOUT;
            break;
        case TICK :
            if (millis() > pushModeDeadline_)
                transition(Personality::enteringPushMode);
            break;
        case SUPERLONG_CLICK_ARMED :
            transition(Personality::wakingUp);
            emit(WAKE_UP);
            break;
    }
}
//------------------------------------------------------------------------------
/** Regular active mode */
void Personality::awake(const Event* e) {
    switch (e->signal) {
        case ENTRY :
            resetDeadlines();
#ifdef DEBUG
            Serial.println(F("Personality::awake"));
#endif
            break;
        case SHORT_CLICK_RELEASED :
            transition(Personality::action);
            emit(ACTION);
            break;
        case SUPERLONG_CLICK_ARMED :
            transition(Personality::fallingAsleep);
            emit(FALL_ASLEEP);
            break;
        case TICK :
            if (millis() >= checkingFacebookDeadline_) {
                emit(FACEBOOK);
                transition(Personality::checkingFacebook);
            }
            else if (millis() >= checkingGmailDeadline_) {
                emit(GMAIL);
                transition(Personality::checkingGmail);
            }
            else if (millis() >= checkingTwitterDeadline_) {
                emit(TWITTER);
                transition(Personality::checkingTwitter);
            }
            else if (millis() >= checkingRssDeadline_) {
                emit(RSS);
                transition(Personality::checkingRss);
            }
            else if (millis() >= checkingFoursquareDeadline_) {
                emit(FOURSQUARE);
                transition(Personality::checkingFoursquare);
            }
            else if (millis() >= checkingSoundCloudDeadline_) {
                emit(SOUNDCLOUD);
                transition(Personality::checkingSoundCloud);
            }
            else if (millis() >= pollInboxDeadline_) {
                transition(Personality::pollingInbox);
            }
            break;
    }
}
//------------------------------------------------------------------------------
/** Checking Facebook */
void Personality::checkingFacebook(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::checking Facebook"));
            break;
#endif
        case STOP :
            internalTransition(Personality::awake);
            Serial.println(F("Personality::awake"));
            checkingFacebookDeadline_ = millis() + CHECK_FACEBOOK_INTERVAL;
            break;
    }
}
//------------------------------------------------------------------------------
/** Checking Foursquare*/
void Personality::checkingFoursquare(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::checking Foursquare"));
            break;
#endif
        case STOP :
            internalTransition(Personality::awake);
            Serial.println(F("Personality::awake"));
            checkingFoursquareDeadline_ = millis() + CHECK_FOURSQUARE_INTERVAL;
            break;
    }
}
//------------------------------------------------------------------------------
/** Checking SoundCloud*/
void Personality::checkingSoundCloud(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::checkingSoundcloud"));
            break;
#endif
        case STOP :
            transition(Personality::awake);
            break;
        case SOUNDCLOUD :
            transition(Personality::playingSoundCloud);
            break;
    }
}//------------------------------------------------------------------------------
/** Playing a sond from SoundCloud*/
void Personality::playingSoundCloud(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::playingSoundcloud"));
            break;
#endif
        case STOP :
            transition(Personality::awake);
            break;
    }
}
//------------------------------------------------------------------------------
/** Checking Gmail */
void Personality::checkingGmail(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::checking Gmail"));
            break;
#endif
        case STOP :
            internalTransition(Personality::awake);
            Serial.println(F("Personality::awake"));
            checkingGmailDeadline_ = millis() + CHECK_GMAIL_INTERVAL;
            break;
    }
}
//------------------------------------------------------------------------------
/** Checking Rss*/
void Personality::checkingRss(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::checking Rss"));
            break;
#endif
        case STOP :
            internalTransition(Personality::awake);
            Serial.println(F("Personality::awake"));
            checkingRssDeadline_ = millis() + CHECK_RSS_INTERVAL;
            break;
    }
}
//------------------------------------------------------------------------------
/** Checking Twitter */
void Personality::checkingTwitter(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::checking Twitter"));
            break;
#endif
        case STOP :
            internalTransition(Personality::awake);
            Serial.println(F("Personality::awake"));
            checkingTwitterDeadline_ = millis() + CHECK_TWITTER_INTERVAL;
            break;
    }
}
//------------------------------------------------------------------------------
void Personality::enteringPushMode(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::enteringPushMode"));
            break;
#endif
        case TICK :
            if (inbox_->enterPushMode())
                transition(Personality::pushMode);
            else {
                inbox_->leavePushMode();
                transition(Personality::asleep);
            }
            break;
    }
}
//------------------------------------------------------------------------------
/** Substate when performing an action before going back to sleep */
void Personality::fallingAsleep(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::fallingAsleep"));
            break;
#endif
        case STOP :
            transition(Personality::asleep);
            break;
    }
}
//------------------------------------------------------------------------------
/** Substate when performing a self-triggered action */
void Personality::pollingInbox(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::pollingInbox"));
            break;
#endif
        case TICK :
        {
            int messageType = inbox_->getMessage();
            if (messageType == INBOX_START_REMOTE) {
                control_->begin(realtime_);
                transition(Personality::remoteControl);
            }
            else {
                internalTransition(Personality::awake);
                Serial.println(F("Personality::awake"));
                pollInboxDeadline_ = millis() + POLL_INTERVAL;
            }
        }
        break;
    }
}
//------------------------------------------------------------------------------
/** Substate when performing a self-triggered action */
void Personality::pushMode(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::pushMode"));
            break;
#endif
        case TICK :
        {
            int messageType = inbox_->getMessage();
            if (messageType == INBOX_START_REMOTE) {
                control_->begin(realtime_);
                transition(Personality::remoteControl);
            }
#ifdef DEBUG
            else if (messageType == INBOX_PING) {
                Serial.println(F("Ping event"));
            }
#endif
            break;
        }
        case SUPERLONG_CLICK_ARMED :
            inbox_->leavePushMode();
            transition(Personality::wakingUp);
            emit(WAKE_UP);
            break;
    }
}
//------------------------------------------------------------------------------
/** Remote control mode */
void Personality::remoteControl(const Event* e) {
    switch (e->signal) {
#ifdef DEBUG
        case ENTRY :
            Serial.println(F("Personality::remoteControl"));
            break;
#endif
        case TICK :
            control_->startNextMotion(millis());
            if (control_->finishedTrajectory()) {
                transition(Personality::pushMode);
            }
            else if (control_->finishedStep()) {
                control_->startNextStep();
            }
            break;
        case SHORT_CLICK_RELEASED :
            inbox_->leavePushMode();
            transition(Personality::fallingAsleep);
            emit(FALL_ASLEEP);
            break;
        case SUPERLONG_CLICK_ARMED :
            inbox_->leavePushMode();
            transition(Personality::wakingUp);
            emit(WAKE_UP);
            break;
    }
}
//------------------------------------------------------------------------------
/** Substate when performing a user-triggered action */
void Personality::wakingUp(const Event* e) {
    switch (e->signal) {
        case ENTRY :
#ifdef DEBUG
            Serial.println(F("Personality::wakingUp"));
#endif
            postActivity(MAX_LEVEL);
            break;
        case STOP :
            transition(Personality::awake);
            break;
        case SHORT_CLICK_RELEASED :
            transition(Personality::awake);
            emit(STOP);
            break;
    }
}
//------------------------------------------------------------------------------
/** Send the current activity level to the reaDIYmate API */
void Personality::postActivity(uint8_t level) {
    char itoaBuffer[3];
    itoa(level, itoaBuffer, 10);
    api_->call(METHOD_POST_ACTIVITY, KEY_ACTIVITY, itoaBuffer);
#ifdef DEBUG
    Serial.print(F("Level: "));
    Serial.println(level);
#endif
}
//------------------------------------------------------------------------------
/** Reset the timers for all recurring actions */
void Personality::resetDeadlines() {
    pollInboxDeadline_ = millis() + POLL_INTERVAL;
    checkingGmailDeadline_ = millis() + CHECK_GMAIL_INTERVAL;
    checkingFacebookDeadline_ = millis() + CHECK_FACEBOOK_INTERVAL;
    checkingTwitterDeadline_ = millis() + CHECK_TWITTER_INTERVAL;
    checkingRssDeadline_ = millis() + CHECK_RSS_INTERVAL;
    checkingFoursquareDeadline_ = millis() + CHECK_FOURSQUARE_INTERVAL;
    checkingSoundCloudDeadline_ = millis() + CHECK_SOUNDCLOUD_INTERVAL;
}
