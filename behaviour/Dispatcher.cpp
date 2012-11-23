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
/* The longest path accepted is a directory name followed by a 8.3 filename. */
static const uint16_t PATH_BUFFER_SIZE = 26;
//------------------------------------------------------------------------------
// Settings
const uint8_t NB_SETTINGS = 26;

const char FACEBOOK_ON[]        PROGMEM = "facebook.on";
const char FACEBOOK_SOUND[]     PROGMEM = "facebook.sound";
const char FACEBOOK_MOTION[]    PROGMEM = "facebook.motion";
const char FACEBOOK_ACTION[]    PROGMEM = "facebook.action";

const char GMAIL_ON[]           PROGMEM = "gmail.on";
const char GMAIL_SOUND[]        PROGMEM = "gmail.sound";
const char GMAIL_MOTION[]       PROGMEM = "gmail.motion";
const char GMAIL_KEYWORD[]      PROGMEM = "gmail.keyword";

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
const char FOURSQUARE_ACTION[]  PROGMEM = "foursquare.action";

const char SOUNDCLOUD_ON[]      PROGMEM = "soundcloud.on";
const char SOUNDCLOUD_OWNER[]   PROGMEM = "soundcloud.owner";
const char SOUNDCLOUD_ACTION[]  PROGMEM = "soundcloud.action";

const char EMAIL_ON[]           PROGMEM = "email.on";

const char* SETTINGS_NAMES[] PROGMEM =
{
    FACEBOOK_ON,
    FACEBOOK_SOUND,
    FACEBOOK_MOTION,
    FACEBOOK_ACTION,
    GMAIL_ON,
    GMAIL_SOUND,
    GMAIL_MOTION,
    GMAIL_KEYWORD,
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
    SOUNDCLOUD_OWNER,
    SOUNDCLOUD_ACTION,
    EMAIL_ON
};
//------------------------------------------------------------------------------
const char STRING_API_HOST[] PROGMEM = "dev.readiymate.com";
const char STRING_API_PATH[] PROGMEM = "/index.php/api/";
//------------------------------------------------------------------------------
// System files
const char DIRECTORY_SOUNDCLOUD[] PROGMEM = "SNDCLD";
const char SOUND_WAKE[]           PROGMEM = "SYSTEM/SYS_WAKE.MP3";
const char SOUND_SLEEP[]          PROGMEM = "SYSTEM/SYS_ZZZZ.MP3";
const char SOUND_CONNECT[]        PROGMEM = "SYSTEM/SYS_CNCT.MP3";
const char SOUND_AOK[]            PROGMEM = "SYSTEM/SYS_AOK.MP3";
const char SOUND_ERR[]            PROGMEM = "SYSTEM/SYS_ERR.MP3";
const char MOTION_ARMSWING[]      PROGMEM = "PUBLIC/ARMSWING.TXT";
//------------------------------------------------------------------------------
Dispatcher::Dispatcher() :
    wifly(UART_WIFLY, PIN_WIFLY_RESET, PIN_WIFLY_GPIO4, PIN_WIFLY_GPIO5,
        PIN_WIFLY_GPIO6),
    led(PIN_GREEN_LED, PIN_RED_LED),
    config(UART_COMPANION, wifly, PIN_SD_CHIPSELECT),
    api(wifly, buffer, BUFFER_SIZE, STRING_API_HOST, STRING_API_PATH),
    settings(SETTINGS_NAMES, NB_SETTINGS, api),
    email(api, settings, EMAIL_ON, sd, PIN_SD_CHIPSELECT),
    gmail(api, settings, GMAIL_ON, GMAIL_MOTION, GMAIL_SOUND, GMAIL_KEYWORD),
    facebook(api, settings, FACEBOOK_MOTION, FACEBOOK_SOUND, FACEBOOK_ON,
        FACEBOOK_ACTION, sd, PIN_SD_CHIPSELECT),
    twitter(api, settings, TWITTER_MOTION, TWITTER_SOUND, TWITTER_ON,
        TWITTER_ACTION, sd, PIN_SD_CHIPSELECT),
    rss(api, settings, RSS_ON, RSS_MOTION, RSS_SOUND, RSS_KEYWORD, RSS_FEED_URL),
    foursquare(api, settings, FOURSQUARE_MOTION, FOURSQUARE_SOUND,
        FOURSQUARE_VENUEID, FOURSQUARE_ACTION, FOURSQUARE_ON),
    audio(PIN_VS1011_DREQ, PIN_VS1011_RESET, PIN_VS1011_XDCS, PIN_VS1011_XCS, sd,
        PIN_SD_CHIPSELECT),
    button(PIN_BUTTON),
    player(audio),
    pusher(wifly, buffer, BUFFER_SIZE),
    inbox(api, pusher),
    control(PIN_SERVO),
    realtime(pusher),
    motion(control, sd, PIN_SD_CHIPSELECT),
    personality(api, led, inbox, control, realtime),
    soundcloud(api, wifly, buffer, BUFFER_SIZE, sd, PIN_SD_CHIPSELECT, settings,
        SOUNDCLOUD_OWNER, SOUNDCLOUD_ACTION, SOUNDCLOUD_ON),
    resources(api, wifly, buffer, BUFFER_SIZE, STRING_API_HOST, sd,
        PIN_SD_CHIPSELECT)
{
}
//------------------------------------------------------------------------------
void Dispatcher::setup() {
    Serial.begin(115200);

    button.initialize();
    led.initialize();
    led.colorOrange();

    wifly.initialize();
    config.synchronize(3000);

    control.write(config.getServoDefaultPosition());
    play_P(SOUND_WAKE, MOTION_ARMSWING);

    char buf[128] = {0};
    config.getApiCredential(buf, 128);
    char* credential = (char*)calloc(strlen(buf) + 1,sizeof(char));
    strncpy(credential, buf, 128 - 1);
    api.setFixedArgs(credential);
    pusher.setKey(config.getPusherKey());
    Serial.print(F("API credential: "));
    Serial.println(credential);
    Serial.println(F("Object configuration restored from the EEPROM."));

    Serial.print(F("Connecting to the reaDIYmate server... "));

    if (api.connect()) {
        Serial.println(F("connection established."));
    }
    else {
        die(F("failed to connect."));
    }

    if (facebook.updateContent()
    && twitter.updateContent()
    && email.updateContent()) {
        Serial.println(F("Predefined content retrieved."));
    }
    else {
        die(F("Failed to retrieve predefined content."));
    }

    if (settings.fetch() >= 0) {
        settings.save();
        Serial.println(F("Application settings updated."));
        Serial.print(settings.getNbSettings());
        Serial.println(F(" new settings saved to the EEPROM."));
    }
    else if (settings.restore() >= 0) {
        Serial.println(F("Application settings restored from the EEPROM."));
        Serial.print(settings.getNbSettings());
        Serial.println(F(" settings restored."));
    }

    // the reaDIYmate is online
    audio.play_P(SOUND_CONNECT);

    if (resources.synchronize()) {
        Serial.println(F("Resource synchronization complete."));
    }
    else {
        die(F("Resource synchronization failed."));
    }

    randomSeed(analogRead(0));
    Serial.println(F("Initialization done."));
    Serial.print(F("----------------------------------------\r\n"));
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
        {
            facebook.updateContent();
            twitter.updateContent();
            email.updateContent();
            if (settings.fetch() >= 0) {
                settings.save();
            }
            resources.synchronize();
            char soundName[PATH_BUFFER_SIZE] = {0};
            strncpy_P(soundName, SOUND_WAKE, PATH_BUFFER_SIZE - 1);
            player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
            break;
        }
        case FALL_ASLEEP :
        {
            char soundName[PATH_BUFFER_SIZE] = {0};
            strncpy_P(soundName, SOUND_SLEEP, PATH_BUFFER_SIZE - 1);
            player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
            break;
        }
        case GMAIL:
            if (gmail.enabled() && gmail.update()) {
                soundName = gmail.getSoundFilename();
                motionName = gmail.getMotionFilename();
                if (soundName != NULL)
                    player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                if (motionName != NULL)
                    motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case FACEBOOK :
            if (facebook.Service::enabled() && facebook.update()) {
                soundName = facebook.getSoundFilename();
                motionName = facebook.getMotionFilename();
                if (soundName != NULL)
                    player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                if (motionName != NULL)
                    motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case TWITTER :
            if (twitter.Service::enabled() && twitter.update()) {
                soundName = twitter.getSoundFilename();
                motionName = twitter.getMotionFilename();
                if (soundName != NULL)
                    player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                if (motionName != NULL)
                    motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case RSS :
            if (rss.enabled() && rss.update()) {
                soundName = rss.getSoundFilename();
                motionName = rss.getMotionFilename();
                if (soundName != NULL)
                    player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                if (motionName != NULL)
                    motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case FOURSQUARE :
            if (foursquare.Action::enabled() && foursquare.update()) {
                soundName = foursquare.getSoundFilename();
                motionName = foursquare.getMotionFilename();
                if (soundName != NULL)
                    player.dispatch(PlayerEvent(PLAY, soundName), playerOut);
                if (motionName != NULL)
                    motion.dispatch(MotionEvent(PLAY, motionName), motionOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            break;
        case SOUNDCLOUD :
            if (soundcloud.alertEnabled() && soundcloud.download(DIRECTORY_SOUNDCLOUD)) {
                personality.dispatch(Event(SOUNDCLOUD), persoOut);
                player.dispatch(PlayerEvent(PLAY, soundcloud.filepath()), playerOut);
            }
            else {
                playerOut.signal = END_OF_FILE;
                motionOut.signal = END_OF_FILE;
            }
            if (!api.connected()) {
                api.connect();
            }
            break;
        case ACTION :
            if (twitter.Action::enabled()) {
                if (twitter.trigger()) {
                    audio.play_P(SOUND_AOK);
                }
                else {
                    audio.play_P(SOUND_ERR);
                }
            }
            if (facebook.Action::enabled()) {
                if (facebook.trigger()) {
                    audio.play_P(SOUND_AOK);
                }
                else {
                    audio.play_P(SOUND_ERR);
                }
            }
            if (foursquare.Action::enabled()) {
                if (foursquare.trigger()) {
                    audio.play_P(SOUND_AOK);
                }
                else {
                    audio.play_P(SOUND_ERR);
                }
            }
            if (email.Action::enabled()) {
                if (email.trigger()) {
                    audio.play_P(SOUND_AOK);
                }
                else {
                    audio.play_P(SOUND_ERR);
                }
            }
            if (soundcloud.enabled()) {
                char directory[PATH_BUFFER_SIZE] = {0};
                strncpy_P(directory, DIRECTORY_SOUNDCLOUD, PATH_BUFFER_SIZE - 1);
                player.dispatch(PlayerEvent(RANDOM, directory), playerOut);
                personality.dispatch(Event(SOUNDCLOUD), persoOut);
            }
            else {
                personality.dispatch(Event(STOP), persoOut);
            }
            break;
        case NOTHING :
            player.dispatch(Event(TICK), playerOut);
            motion.dispatch(Event(TICK), motionOut);
            break;
    }

    if (playerOut.signal == END_OF_FILE) {
        motion.dispatch(Event(STOP));
        player.dispatch(Event(STOP));
        personality.dispatch(Event(STOP), persoOut);
    }
}
//------------------------------------------------------------------------------
void Dispatcher::play(const char* soundName, const char* motionName) {
    Event motionOut;
    motion.dispatch(MotionEvent(PLAY, motionName), motionOut);

    Event playerOut;
    player.dispatch(PlayerEvent(PLAY, soundName), playerOut);

    bool motionFinished = false;
    bool soundFinished = false;

    do {
        if (motionOut.signal == END_OF_FILE)
            motionFinished = true;
        motion.dispatch(Event(TICK), motionOut);
        if (playerOut.signal == END_OF_FILE)
            soundFinished = true;
        player.dispatch(Event(TICK), playerOut);
    } while (!(motionFinished && soundFinished));
}
//------------------------------------------------------------------------------
void Dispatcher::play_P(PGM_P soundName, PGM_P motionName) {
    char buffer[PATH_BUFFER_SIZE];

    Event motionOut;
    memset(buffer, 0x00, PATH_BUFFER_SIZE);
    strncpy_P(buffer, motionName, PATH_BUFFER_SIZE - 1);
    motion.dispatch(MotionEvent(PLAY, buffer), motionOut);

    Event playerOut;
    memset(buffer, 0x00, PATH_BUFFER_SIZE);
    strncpy_P(buffer, soundName, PATH_BUFFER_SIZE - 1);
    player.dispatch(PlayerEvent(PLAY, buffer), playerOut);

    bool motionFinished = false;
    bool soundFinished = false;

    do {
        if (motionOut.signal == END_OF_FILE)
            motionFinished = true;
        motion.dispatch(Event(TICK), motionOut);
        if (playerOut.signal == END_OF_FILE)
            soundFinished = true;
        player.dispatch(Event(TICK), playerOut);
    } while (!(motionFinished && soundFinished));
}
//------------------------------------------------------------------------------
void Dispatcher::die(__FlashStringHelper* message) {
    Serial.println(message);
    // the reaDIYmate is offline
    led.colorRed();
    // hang forever
    while (true);
}
