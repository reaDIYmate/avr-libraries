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
#include <Configuration.h>
//------------------------------------------------------------------------------
// reaDIYmate Companion commands
/** End character */
uint8_t const COMMAND_END_CHAR = '}';
/** UART timeout */
uint32_t const WIZARD_TIMEOUT = 5000;
//------------------------------------------------------------------------------
// Strings used to communication with the reaDIYmate Companion
/** SSID of the WLAN */
const char PROGMEM WIZARD_KEY_SSID[] = "ssid";
/** Passphrase for the WLAN */
const char PROGMEM WIZARD_KEY_PASSPHRASE[] = "phrase";
/** Connection mode */
const char PROGMEM WIZARD_KEY_MODE[] = "mode";
/** DHCP connection mode */
const char PROGMEM WIZARD_KEY_DHCP[] = "dhcp";
/** Static IP address */
const char PROGMEM WIZARD_KEY_IP[] = "ip";
/** Gateway IP */
const char PROGMEM WIZARD_KEY_GATEWAY[] = "gateway";
/** Network mask */
const char PROGMEM WIZARD_KEY_NETMASK[] = "mask";
/** User name of the reaDIYmate website */
const char PROGMEM WIZARD_KEY_USER[] = "user";
/** Password of the reaDIYmate website */
const char PROGMEM WIZARD_KEY_PASSWORD[] = "token";
/**  Key for the command */
const char PROGMEM COMMAND_TYPE[] = "cmd";
/** DeviceID mode */
const char PROGMEM COMMAND_DEVICEID[] = "id";
/** Configuration mode */
const char PROGMEM COMMAND_CONFIGURATION[] = "config";
/** Factory mode */
const char PROGMEM COMMAND_FACTORY[] = "factory";
/** Start-up signal */
const char PROGMEM WIZARD_STARTUP[] = "reaDIYmate\n";
//------------------------------------------------------------------------------
/** Format string used to construct the credential for API calls */
const char API_CREDENTIAL_FORMAT[] PROGMEM = "id=%s&user=%s&token=%s";
//------------------------------------------------------------------------------
/**
 * Construct an instance of Configuration.
 *
 * \param[in] companion The serial port that is connected to the companion.
 * \param[in] wifly The Wifly object to use for communications.
 */
Configuration::Configuration(HardwareSerial &companion, Wifly &wifly, StatusLed &led) :
    SerialStream(companion, WIZARD_TIMEOUT),
    wifly_(&wifly),
    led_(&led),
    username_(NULL),
    password_(NULL),
    key_(NULL),
    secret_(NULL),
    channel_(NULL)
{
    restoreDeviceId();
    restoreUserAndPass();
    restorePusher();
}
//------------------------------------------------------------------------------
/** Free the memory allocated for application parameters */
Configuration::~Configuration() {
    if (username_ != NULL) {
        free(username_);
        username_ = NULL;
    }
    if (password_ != NULL) {
        free(password_);
        password_ = NULL;
    }
    if (key_ != NULL) {
        free(key_);
        key_ = NULL;
    }
    if (secret_ != NULL) {
        free(secret_);
        secret_ = NULL;
    }
    if (channel_ != NULL) {
        free(channel_);
        channel_ = NULL;
    }
}
//------------------------------------------------------------------------------
/**
 * Using the device ID, the username and the password, generate a string that
 * will be appended to every request sent to the reaDIYmate API.
 *
 * \param[out] buffer The location where the credential string should be written.
 * \param[out] bufferSize The maximum length of the string.
 */
void Configuration::getApiCredential(char* buffer, uint8_t bufferSize) {
    memset(buffer, 0x00, bufferSize);
    snprintf_P(
        buffer,
        bufferSize,
        API_CREDENTIAL_FORMAT,
        deviceId_,
        username_,
        password_
    );
}
//------------------------------------------------------------------------------
/** Read the username and password sent by the Companion */
void Configuration::readUserAndPass(char* buffer, uint8_t bufferSize) {
    JsonStream json = JsonStream(buffer, bufferSize);

    char newUsername[64] = {0};
    char newPassword[128] = {0};

    json.getStringByName_P(WIZARD_KEY_USER, newUsername, 64);
    json.getStringByName_P(WIZARD_KEY_PASSWORD, newPassword, 128);

    if (strcmp(username_, newUsername) != 0 || strcmp(password_, newPassword) != 0) {
        username_ = newUsername;
        password_ = newPassword;
        saveUserAndPass();
    }
}
//------------------------------------------------------------------------------
/** Read the Wi-Fi settings sent by the Companion and update the WiFly config */
void Configuration::readWifiSettings(char* buffer, uint8_t bufferSize) {


    JsonStream json = JsonStream(buffer, bufferSize);

    // parse the individual settings one by one
    char mode[8] = {0};
    json.getStringByName_P(WIZARD_KEY_MODE, mode, 8);
    char ip[16] = {0};
    json.getStringByName_P(WIZARD_KEY_IP, ip, 16);
    char mask[16] = {0};
    json.getStringByName_P(WIZARD_KEY_NETMASK, mask, 16);
    char gateway[16] = {0};
    json.getStringByName_P(WIZARD_KEY_GATEWAY, gateway, 16);
    char ssid[32] = {0};
    json.getStringByName_P(WIZARD_KEY_SSID, ssid, 32);
    char passphrase[64] = {0};
    json.getStringByName_P(WIZARD_KEY_PASSPHRASE, passphrase, 64);

    // check the validity of the new settings
    bool dhcp = (strcmp_P(mode, WIZARD_KEY_DHCP) == 0);
    if (ssid == NULL || passphrase == NULL)
        return;
    if (dhcp == false && (ip == NULL || mask == NULL || gateway == NULL))
        return;

    // update the configuration of the Wi-Fi module
    if (dhcp == true) {
        wifly_->setConfig(ssid, passphrase);
    }
    else {
        wifly_->setConfig(ssid, passphrase, ip, mask, gateway);
    }
}
//------------------------------------------------------------------------------
/**
  * Restore the device ID from the EEPROM to the SRAM
  *
  * \note The device ID is the MAC address of the Wi-Fi module stored in ASCII.
  */
void Configuration::restoreDeviceId() {
    memset(deviceId_, 0x00, 13);
    eeprom_read_block((void*)deviceId_, (const void*)EEPROM_DEVICEID, 12);
}
//------------------------------------------------------------------------------
/** Restore username and password from the EEPROM to the SRAM */
void Configuration::restorePusher() {
    char buffer[21];

    // read key
    memset(buffer, 0x00, 21);
    eeprom_read_block((void*)buffer, (const void*)EEPROM_PUSHER_KEY, 21);
    key_ = (char*)calloc(strlen(buffer) + 1,sizeof(char));
    strcpy(key_, buffer);

    // read secret
    memset(buffer, 0x00, 21);
    eeprom_read_block((void*)buffer, (const void*)EEPROM_PUSHER_SECRET, 21);
    secret_ = (char*)calloc(strlen(buffer) + 1,sizeof(char));
    strcpy(secret_, buffer);

    // read channel
    memset(buffer, 0x00, 21);
    eeprom_read_block((void*)buffer, (const void*)EEPROM_PUSHER_CHANNEL, 21);
    channel_ = (char*)calloc(strlen(buffer) + 1,sizeof(char));
    strcpy(channel_, buffer);
}
//------------------------------------------------------------------------------
/** Restore username and password from the EEPROM to the SRAM */
void Configuration::restoreUserAndPass() {
    char buffer[64];

    // read username
    memset(buffer, 0x00, 64);
    eeprom_read_block((void*)buffer, (const void*)EEPROM_USERNAME, 32);
    username_ = (char*)calloc(strlen(buffer) + 1,sizeof(char));
    strcpy(username_, buffer);

    // read password
    memset(buffer, 0x00, 64);
    eeprom_read_block((void*)buffer, (const void*)EEPROM_PASSWORD, 64);
    password_ = (char*)calloc(strlen(buffer) + 1,sizeof(char));
    strcpy(password_, buffer);
}
//------------------------------------------------------------------------------
/** Save Pusher key/secret/channel from the SRAM to the EEPROM */
void Configuration::savePusher() {
    char buffer[21];

    // save key
    memset(buffer, 0x00, 21);
    strcpy(buffer, key_);
    eeprom_write_block((const void*)buffer, (void*)EEPROM_PUSHER_KEY, 21);

    // save secret
    memset(buffer, 0x00, 21);
    strcpy(buffer, secret_);
    eeprom_write_block((const void*)buffer, (void*)EEPROM_PUSHER_SECRET, 21);

    // save channel
    memset(buffer, 0x00, 21);
    strcpy(buffer, channel_);
    eeprom_write_block((const void*)buffer, (void*)EEPROM_PUSHER_CHANNEL, 21);
}
//------------------------------------------------------------------------------
/** Save username and password from the SRAM to the EEPROM */
void Configuration::saveUserAndPass() {
    char buffer[64];

    // save username
    memset(buffer, 0x00, 64);
    strcpy(buffer, username_);
    eeprom_write_block((const void*)buffer, (void*)EEPROM_USERNAME, 32);

    // save password
    memset(buffer, 0x00, 64);
    strcpy(buffer, password_);
    eeprom_write_block((const void*)buffer, (void*)EEPROM_PASSWORD, 64);
}
//------------------------------------------------------------------------------
/** Save the device ID the SRAM to the EEPROM */
void Configuration::saveDeviceId() {
    eeprom_write_block((const void*)deviceId_, (void*)EEPROM_DEVICEID, 12);
}
//------------------------------------------------------------------------------
/** Send the device ID to the Companion */
void Configuration::sendDeviceId() {
    print(deviceId_);
    write('\n');
}
//------------------------------------------------------------------------------
/**
 * Attempt to synchronize with the reaDIYmate Companion via a Serial port
 *
 * \param[in] timeout Timeout used to decide when to abort the synchronization.
 */
void Configuration::synchronize(uint16_t timeout) {

    led_->colorOrange();
    char buffer[512] = {0};
    write_P(WIZARD_STARTUP);

    uint32_t deadline = millis() + timeout;
    while (millis() < deadline) {
        int nbBytes = readBytesUntil(COMMAND_END_CHAR, buffer, 512);
        JsonStream json = JsonStream(buffer, nbBytes);

        char cmd[16] = {0};
        json.getStringByName_P(COMMAND_TYPE, cmd, 16);

        if (strcmp_P(cmd, COMMAND_DEVICEID) == 0) {
            sendDeviceId();
        }
        else if (strcmp_P(cmd, COMMAND_CONFIGURATION) == 0) {
            readWifiSettings(buffer, nbBytes);
            readUserAndPass(buffer, nbBytes);
            return;
        }
        else if (strcmp_P(cmd, COMMAND_FACTORY) == 0) {
            wifly_->setFirstConfig();
            wifly_->getDeviceId(deviceId_);
            saveDeviceId();
            Serial.println(F("Factory configuration"));
            return;
        }
    }
    led_->colorGreen();
}
