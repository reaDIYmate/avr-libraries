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
const char KEY_TO[] PROGMEM = "to";
const char KEY_SUBJECT[] PROGMEM = "subject";
const char KEY_BODY[] PROGMEM = "body";
const char KEY_STATUS[] PROGMEM = "status";
const char SD_FILE[] = "EMAIL.TXT";
//------------------------------------------------------------------------------
Email::Email(Api &api, SdFat &sd, uint8_t sdChipSelectPin) :
    api_(&api),
    sd_(&sd),
    sdChipSelectPin_(sdChipSelectPin)
{
}
//------------------------------------------------------------------------------
bool Email::sendEmail() {
    char file[256] = {0};
    char to[32] = {0};
    char subject[32] = {0};
    char body[140] = {0};
    
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_)) {
        sd_->initErrorHalt();
        return false;
    }
    if (!open(SD_FILE, O_READ)) {
        return false;
    }
    rewind();
    int nbBytes = read(file, 256);
    JsonStream json = JsonStream(file, nbBytes);
    json.getStringByName_P(KEY_TO, to, 32);
    json.getStringByName_P(KEY_SUBJECT, subject, 32);
    json.getStringByName_P(KEY_BODY, body, 140);
    api_->call(STRING_API_EMAIL_SEND, KEY_TO, to, KEY_SUBJECT, subject, KEY_BODY, body);
    char buffer[4];
    api_->getStringByName_P(KEY_STATUS, buffer, 4);
    if (strcmp("0", buffer) == 0){
        close();
        return true;
    }
    close();
    return false;
}