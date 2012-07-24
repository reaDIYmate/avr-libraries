/* reaDIYmate AVR library
 * Written by Pierre Bouchet
 * Copyright (C) 2011-2012 reaDIYmate
 *
 * This file is part of the reaDIYmate library
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
#include "Dispatcher.h"
//------------------------------------------------------------------------------
#define PIN_VS1011_DREQ 70
#define PIN_VS1011_XCS 71
#define PIN_VS1011_XDCS 72
#define PIN_VS1011_RESET 73
#define PIN_WIFLY_GPIO4 74
#define PIN_WIFLY_GPIO5 75
#define PIN_WIFLY_GPIO6 76
#define PIN_SD_CHIPSELECT 77
#define PIN_GREEN_LED 78
#define PIN_RED_LED 79
#define PIN_WIFLY_RESET 49

#define PIN_BUTTON 2
#define PIN_SERVO 4
#define UART_WIFLY Serial1
#define UART_COMPANION Serial
//------------------------------------------------------------------------------
// Settings
const uint8_t NB_SETTINGS = 21;

const char FACEBOOK_ON[]        PROGMEM = "facebook.on";
const char FACEBOOK_SOUND[]     PROGMEM = "facebook.sound";
const char FACEBOOK_MOTION[]    PROGMEM = "facebook.motion";
const char FACEBOOK_ACTION[]    PROGMEM = "facebook.action";

const char GMAIL_ON[]           PROGMEM = "gmail.on";
const char GMAIL_SOUND[]        PROGMEM = "gmail.sound";
const char GMAIL_MOTION[]       PROGMEM = "gmail.motion";

const char TWITTER_ON[]         PROGMEM = "twitter.on";
const char TWITTER_SOUND[]      PROGMEM = "twitter.sound";
const char TWITTER_MOTION[]     PROGMEM = "twitter.motion";
const char TWITTER_ACTION[]     PROGMEM = "twitter.action";

const char RSS_ON[]             PROGMEM = "rss.on";
const char RSS_SOUND[]          PROGMEM = "rss.sound";
const char RSS_MOTION[]         PROGMEM = "rss.motion";
const char RSS_KEYWORD[]        PROGMEM = "rss.keyword";
const char RSS_FEED_URL[]       PROGMEM = "rss.feed_url";

const char FOURSQUARE_ON[]      PROGMEM = "foursquare.on";
const char FOURSQUARE_SOUND[]   PROGMEM = "foursquare.sound";
const char FOURSQUARE_MOTION[]  PROGMEM = "foursquare.motion";
const char FOURSQUARE_VENUEID[] PROGMEM = "foursquare.venueId";
const char FOURSQUARE_ACTION[] PROGMEM = "foursquare.action";

const char SOUNDCLOUD_ON[]      PROGMEM = "soundcloud.on";
const char SOUNDCLOUD_SOUND[]   PROGMEM = "soundcloud.sound";
const char SOUNDCLOUD_MOTION[]  PROGMEM = "soundcloud.motion";

const char* SETTINGS_NAMES[] PROGMEM =
{
    FACEBOOK_ON,
    FACEBOOK_SOUND,
    FACEBOOK_MOTION,
    FACEBOOK_ACTION,
    GMAIL_ON,
    GMAIL_SOUND,
    GMAIL_MOTION,
    TWITTER_ON,
    TWITTER_SOUND,
    TWITTER_MOTION,
    TWITTER_ACTION,
    RSS_ON,
    RSS_SOUND,
    RSS_MOTION,
    RSS_KEYWORD,
    RSS_FEED_URL,
    FOURSQUARE_ON,
    FOURSQUARE_SOUND,
    FOURSQUARE_MOTION,
    FOURSQUARE_VENUEID,
    FOURSQUARE_ACTION,
    SOUNDCLOUD_ON,
    SOUNDCLOUD_SOUND,
    SOUNDCLOUD_MOTION
};
//------------------------------------------------------------------------------
const char STRING_API_HOST[] PROGMEM = "dev.readiymate.com";
const char STRING_API_PATH[] PROGMEM = "/index.php/api/";
//------------------------------------------------------------------------------
Dispatcher::Dispatcher() :
    wifly(
        UART_WIFLY,
        PIN_WIFLY_RESET,
        PIN_WIFLY_GPIO4,
        PIN_WIFLY_GPIO5,
        PIN_WIFLY_GPIO6
    ),
    led(
        PIN_GREEN_LED,
        PIN_RED_LED
    ),
    config(
        UART_COMPANION,
        wifly,
        led
    ),
    api(
        wifly,
        buffer,
        BUFFER_SIZE,
        STRING_API_HOST,
        STRING_API_PATH
    ),
    settings(
        SETTINGS_NAMES,
        NB_SETTINGS,
        api
    ),
    gmail(
        api,
        settings,
        GMAIL_ON,
        GMAIL_MOTION,
        GMAIL_SOUND
    ),
    facebook(
        api,
        settings,
        FACEBOOK_ON,
        FACEBOOK_MOTION,
        FACEBOOK_SOUND,
        FACEBOOK_ACTION
    ),
    twitter(
        api,
        settings,
        TWITTER_ON,
        TWITTER_MOTION,
        TWITTER_SOUND,
        TWITTER_ACTION
    ),
    rss(
        api,
        settings,
        RSS_ON,
        RSS_MOTION,
        RSS_SOUND,
        RSS_KEYWORD,
        RSS_FEED_URL
    ),
    foursquare(
        api,
        settings,
        FOURSQUARE_ON,
        FOURSQUARE_MOTION,
        FOURSQUARE_SOUND,
        FOURSQUARE_VENUEID,
        FOURSQUARE_ACTION
    ),
    audio(
        PIN_VS1011_DREQ,
        PIN_VS1011_RESET,
        PIN_VS1011_XDCS,
        PIN_VS1011_XCS,
        sd,
        PIN_SD_CHIPSELECT
    ),
    button(
        PIN_BUTTON,
        led
    ),
    player(audio),
    pusher(
        wifly,
        buffer,
        BUFFER_SIZE
    ),
    inbox(api, pusher),
    control(PIN_SERVO),
    realtime(pusher),
    motion(
        control,
        sd,
        PIN_SD_CHIPSELECT
    ),
    personality(
        api,
        inbox,
        control,
        realtime
    )
{
}
//------------------------------------------------------------------------------
void Dispatcher::setup() {
    Serial.begin(9600);

    button.initialize();
    led.initialize();
    led.colorRed();

    wifly.initialize();
    config.synchronize(3000);

    char buf[128] = {0};
    config.getApiCredential(buf, 128);
    char* credential = (char*)calloc(strlen(buf) + 1,sizeof(char));
    strcpy(credential, buf);
    api.setFixedArgs(credential);
    pusher.setConfig(config.getPusherKey(), config.getPusherSecret(), config.getPusherChannel());
    Serial.print(F("API credential: "));
    Serial.println(credential);
    Serial.println(F("Object configuration restored from EEPROM."));

    Serial.println(F("Connecting to the reaDIYmate server..."));
    led.colorOrange();
    bool restore = true;
    if (api.connect()) {
        Serial.println(F("Connection to the reaDIYmate server established."));
        if (settings.fetch() >= 0) {
            settings.save();
            restore = false;
            Serial.println(F("Application settings updated."));
            Serial.print(settings.getNbSettings());
            Serial.println(F(" settings saved."));
        }
    }
    if (restore) {
        if (settings.restore() >= 0) {
            Serial.println(F("Application settings restored from EEPROM."));
            Serial.print(settings.getNbSettings());
            Serial.println(F(" settings restored."));
        }
    }
    led.colorGreen();
    randomSeed(analogRead(0));
    Serial.println(F("Initialization done."));
    Serial.print(F("----------------------------------------\r\n"));
    audio.play("JINGLE6B.MP3");
    led.colorNothing();
    personality.initialize();
}
//------------------------------------------------------------------------------
void Dispatcher::loop() {
    Event buttonOut;
    switch (digitalRead(PIN_BUTTON)) {
        case LOW :
            button.dispatch(Event(CONTACT_DETECTED), buttonOut);
            break;
        case HIGH :
            button.dispatch(Event(NO_CONTACT_DETECTED), buttonOut);
            break;
    }

    Event persoOut;
    switch (buttonOut.signal) {
        case SHORT_CLICK_RELEASED :
            personality.dispatch(Event(SHORT_CLICK_RELEASED), persoOut);
            break;
        case SUPERLONG_CLICK_ARMED :
            personality.dispatch(Event(SUPERLONG_CLICK_ARMED), persoOut);
            break;
        case NOTHING :
            personality.dispatch(Event(TICK), persoOut);
            break;
    }

    Event playerOut,motionOut;
    char* soundName;
    char* motionName;
    switch (persoOut.signal) {
        case WAKE_UP :
            player.dispatch(PlayerEvent(PLAY, "JINGLE6.MP3"), playerOut);
            break;
        case FALL_ASLEEP :
            player.dispatch(PlayerEvent(PLAY, "YAWN.MP3"), playerOut);
            break;
        case GMAIL:
            if (gmail.update()) {
                soundName = gmail.getSoundFilename();
                motionName = gmail.getMotionFilename();
                player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case FACEBOOK :
            if (facebook.update()) {
                soundName = facebook.getSoundFilename();
                motionName = facebook.getMotionFilename();
                player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case TWITTER :
            if (twitter.update()) {
                soundName = twitter.getSoundFilename();
                motionName = twitter.getMotionFilename();
                player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case RSS :
            if (rss.update()) {
                soundName = rss.getSoundFilename();
                motionName = rss.getMotionFilename();
                player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case FOURSQUARE :
            if (foursquare.update()) {
                soundName = foursquare.getSoundFilename();
                motionName = foursquare.getMotionFilename();
                player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case ACTION :
            if(twitter.postStatus()){
                Serial.println("Twitter OK");
            }
            if(facebook.postStatus()){
                Serial.println("Facebook OK");
            }
            if(foursquare.checkin()){
                Serial.println("Foursquare OK");
            }
            personality.dispatch(Event(STOP), persoOut);
            break;
        case NOTHING :
            player.dispatch(Event(TICK), playerOut);
            motion.dispatch(Event(TICK), motionOut);
            break;
    }

    if (playerOut.signal == END_OF_FILE || motionOut.signal == END_OF_FILE) {
        motion.dispatch(Event(STOP));
        player.dispatch(Event(STOP));
        personality.dispatch(Event(STOP), persoOut);
    }
}
