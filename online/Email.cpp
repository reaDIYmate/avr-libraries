/* reaDIYmate AVR library
 * Written by Christopher Schindler
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
#include "Email.h"
//------------------------------------------------------------------------------
const char STRING_API_EMAIL_SEND[] PROGMEM = "email/send";
const char STRING_API_EMAIL_AUTO[] PROGMEM = "email/auto";
const char KEY_TO[] PROGMEM = "to";
const char KEY_SUBJECT[] PROGMEM = "subject";
const char KEY_BODY[] PROGMEM = "body";
const char SETTINGS_UP_TO_DATE[] PROGMEM = "null";
const char END_CHAR[] PROGMEM = "}";
const char SD_FILE[] PROGMEM = "EMAIL.TXT";
//------------------------------------------------------------------------------
Email::Email(Api &api, Settings &settings, PGM_P enabled, SdFat &sd,
    uint8_t sdChipSelectPin) :
    ContentAction(api, settings, enabled, sd, sdChipSelectPin, SD_FILE)
{
}
//------------------------------------------------------------------------------
bool Email::perform() {
    char file[256] = {0};
    int nbBytes = read(file, 256);

    JsonStream json = JsonStream(file, nbBytes);
    char to[32] = {0};
    json.getStringByName_P(KEY_TO, to, 32);
    char subject[64] = {0};
    json.getStringByName_P(KEY_SUBJECT, subject, 64);
    char body[140] = {0};
    json.getStringByName_P(KEY_BODY, body, 140);

    int nBytes = api_->call(STRING_API_EMAIL_SEND, KEY_TO, to, KEY_SUBJECT,
        subject, KEY_BODY, body);
    return (nBytes > 0);
}
//------------------------------------------------------------------------------
bool Email::updateContent() {
    char buffer[256] = {0};
    int nBytes = api_->call(STRING_API_EMAIL_AUTO);
    if (nBytes < 0)
        return false;

    api_->rewind();
    if (api_->find_P(SETTINGS_UP_TO_DATE)) {
        return true;
    }
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    char filename[13] = {0};
    strcpy_P(filename, filename_);
    if (sd_->exists(filename)){
        sd_->remove(filename);
    }
    if (!open(filename, O_CREAT | O_WRITE)) {
        return false;
    }
    api_->rewind();
    nBytes = api_->readBytesUntil_P(END_CHAR, buffer, 256);
    if (nBytes > -1) {
        write(buffer, nBytes);
        sync();
    }
    close();
    return (nBytes > -1);
}
