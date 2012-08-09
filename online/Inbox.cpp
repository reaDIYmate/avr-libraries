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
#include <Inbox.h>
//------------------------------------------------------------------------------
 // reaDIYmate API strings
/** Name of the API method used to retrieve inbox messages */
const char PROGMEM METHOD_INBOX_READ[] = "inbox/read";
/** Name of the API method used to generate the Pusher channel */
const char PROGMEM METHOD_PUSHER_CHANNEL[] = "pusher/generate";
/** Key corresponding to the message type in the API response */
const char PROGMEM KEY_MESSAGE_TYPE[] = "type";
/** Key corresponding to the channel name in the API response */
const char PROGMEM KEY_CHANNEL_NAME[] = "channel";
//------------------------------------------------------------------------------
/**
 * Construct an instance of Inbox.
 *
 * \param[in] api The instance of Api to use for API calls.
 * \param[in] pusherClient The Pusher client to receive events from.
 */
Inbox::Inbox(Api &api, PusherClient &pusher) :
    api_(&api),
    pusher_(&pusher),
    stream_((JsonStream*)&api)
{
}
//------------------------------------------------------------------------------
bool Inbox::enterPushMode() {
    if (stream_ == pusher_ && pusher_->connected()) {
        return true;
    }
    api_->call(METHOD_PUSHER_CHANNEL);
    char channel[21] = {0};
    api_->getStringByName_P(KEY_CHANNEL_NAME, channel, 21);
    api_->disconnect();
    stream_ = pusher_;

    // connect to Pusher
    if (!pusher_->connect()) {
        return false;
    }
    // subscribe to channel
    if (!pusher_->subscribe(channel)) {
        pusher_->disconnect();
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
/**
 * Fetch a message from the inbox and determine its type.
 *
 * \return The message type is returned.
 */
int Inbox::getMessage() {
    if (stream_ == api_) {
        if (!api_->call(METHOD_INBOX_READ))
            return INBOX_NOTHING;
    }
    else if (stream_ == pusher_) {
        if (!pusher_->hasNextEvent())
            return INBOX_NOTHING;
        if (pusher_->ping()) {
            pusher_->pong();
            return INBOX_PING;
        }
    }
    else
        return INBOX_NOTHING;

    return stream_->getIntegerByName_P(KEY_MESSAGE_TYPE);
}
//------------------------------------------------------------------------------
bool Inbox::leavePushMode() {
    if (stream_ == api_ && api_->connected())
        return true;
    else {
        pusher_->disconnect();
        stream_ = api_;
    }
    return api_->connect();
}
