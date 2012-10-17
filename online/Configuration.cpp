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
#define DEBUG
//------------------------------------------------------------------------------
#ifdef DEBUG
    #define DEBUG_LOG(str) Serial.println(F(str))
#else
    #define DEBUG_LOG(str)
#endif
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
const char PROGMEM WIZARD_DHCP[] = "dhcp";
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
/** Key for Pusher */
const char PROGMEM WIZARD_KEY_KEY[] = "key";
/** Secret key for Pusher */
const char PROGMEM WIZARD_KEY_SECRET[] = "secret";
/** Channel for Pusher */
const char PROGMEM WIZARD_KEY_CHANNEL[] = "channel";
/**  Key for the command */
const char PROGMEM COMMAND_TYPE[] = "cmd";
/** DeviceID mode */
const char PROGMEM COMMAND_DEVICEID[] = "id";
/** Configuration mode */
const char PROGMEM COMMAND_AUTH[] = "auth";
/** Configuration mode */
const char PROGMEM COMMAND_BOOTLOADER[] = "bootloader";
/** Factory mode */
const char PROGMEM COMMAND_FACTORY[] = "factory";
/** Update Wifly firmware mode */
const char PROGMEM COMMAND_UPDATE[] = "update";
/** Pusher mode */
const char PROGMEM COMMAND_PUSHER[] = "pusher";
/** WLAN config */
const char PROGMEM COMMAND_WLAN[] = "wlan";
/** Format SD card */
const char PROGMEM COMMAND_FORMAT[] = "format";
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
Configuration::Configuration(HardwareSerial &companion, Wifly &wifly,
    StatusLed &led, uint8_t sdChipSelectPin) :
    SerialStream(companion, WIZARD_TIMEOUT),
    wifly_(&wifly),
    led_(&led),
    sdChipSelectPin_(sdChipSelectPin),
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
/*
 * This method writes the "magic" value to the last EEPROM word. The reaDIYboot
 * bootloader always checks this location at start-up. If the value is anything
 * else than 0x232e, reaDIYboot will act as a regular STK500 bootloader.
 */
 void Configuration::enableBootloader() {
    eeprom_write_word((uint16_t*)(0xFFF - 1), 0x232e);
}
//------------------------------------------------------------------------------
bool Configuration::formatSdCard() {
    Sd2Card card;
    cache_t cache;
    uint32_t cardSizeBlocks;
    uint16_t cardCapacityMB;
    // initialize SD card
    if (!card.init(SPI_HALF_SPEED, sdChipSelectPin_))
      return false;
    cardSizeBlocks = card.cardSize();
    if (cardSizeBlocks == 0)
      return false;
    cardCapacityMB = (cardSizeBlocks + 2047)/2048;

    Serial.println(F("Erasing..."));

    uint32_t const ERASE_SIZE = 262144L;
    uint32_t firstBlock = 0;
    uint32_t lastBlock;
    uint16_t n = 0;
    // flash erase all data
    do {
        lastBlock = firstBlock + ERASE_SIZE - 1;
        if (lastBlock >= cardSizeBlocks)
            lastBlock = cardSizeBlocks - 1;
        if (!card.erase(firstBlock, lastBlock))
            return false;
        firstBlock += ERASE_SIZE;
    } while (firstBlock < cardSizeBlocks);
    if (!card.readBlock(0, cache.data))
      return false;

    Serial.println(F("Formatting..."));

    uint8_t numberOfHeads;
    uint8_t sectorsPerTrack;
    uint16_t reservedSectors;
    uint8_t sectorsPerCluster;
    uint32_t fatStart;
    uint32_t fatSize;
    uint32_t dataStart;
    // initialize appropriate sizes for SD capacity
    if (cardCapacityMB <= 6)
        return false;
    else if (cardCapacityMB <= 16)
        sectorsPerCluster = 2;
    else if (cardCapacityMB <= 32)
        sectorsPerCluster = 4;
    else if (cardCapacityMB <= 64)
        sectorsPerCluster = 8;
    else if (cardCapacityMB <= 128)
        sectorsPerCluster = 16;
    else if (cardCapacityMB <= 1024)
        sectorsPerCluster = 32;
    else
        sectorsPerCluster = 64;

    Serial.print(F("Blocks/Cluster: "));
    Serial.println(sectorsPerCluster);

    // set fake disk geometry
    sectorsPerTrack = cardCapacityMB <= 256 ? 32 : 63;
    if (cardCapacityMB <= 16)
        numberOfHeads = 2;
    else if (cardCapacityMB <= 32)
        numberOfHeads = 4;
    else if (cardCapacityMB <= 128)
        numberOfHeads = 8;
    else if (cardCapacityMB <= 504)
        numberOfHeads = 16;
    else if (cardCapacityMB <= 1008)
        numberOfHeads = 32;
    else if (cardCapacityMB <= 2016)
        numberOfHeads = 64;
    else if (cardCapacityMB <= 4032)
        numberOfHeads = 128;
    else
        numberOfHeads = 255;

    // check card type
    if (card.type() == SD_CARD_TYPE_SDHC)
        return false;

    uint16_t const BU16 = 128;
    uint16_t const BU32 = 8192;
    uint8_t partType;
    uint32_t relSector;
    uint32_t partSize;
    uint32_t nc;
    // make FAT16 volume
    for (dataStart = 2 * BU16;; dataStart += BU16) {
        nc = (cardSizeBlocks - dataStart)/sectorsPerCluster;
        fatSize = (nc + 2 + 255)/256;
        uint32_t r = BU16 + 1 + 2 * fatSize + 32;
        if (dataStart < r)
            continue;
        relSector = dataStart - r + BU16;
        break;
    }

    // check valid cluster count for FAT16 volume
    if (nc < 4085 || nc >= 65525)
        return false;
    reservedSectors = 1;
    fatStart = relSector + reservedSectors;
    partSize = nc * sectorsPerCluster + 2 * fatSize + reservedSectors + 32;
    if (partSize < 32680)
        partType = 0X01;
    else if (partSize < 65536)
        partType = 0X04;
    else
        partType = 0X06;

    // format the Master Boot Record
    memset(&cache, 0, sizeof(cache));
    cache.mbr.mbrSig0 = BOOTSIG0;
    cache.mbr.mbrSig1 = BOOTSIG1;
    part_t* p = cache.mbr.part;
    p->boot = 0;
    uint16_t c = relSector / (numberOfHeads * sectorsPerTrack);
    if (c > 1023)
        return false;
    p->beginCylinderHigh = c >> 8;
    p->beginCylinderLow = c & 0XFF;
    p->beginHead = (relSector % (numberOfHeads * sectorsPerTrack)) /
        sectorsPerTrack;
    p->beginSector = (relSector % sectorsPerTrack) + 1;
    p->type = partType;
    uint32_t endLbn = relSector + partSize - 1;
    c = endLbn / (numberOfHeads * sectorsPerTrack);
    if (c <= 1023) {
        p->endCylinderHigh = c >> 8;
        p->endCylinderLow = c & 0XFF;
        p->endHead = (endLbn % (numberOfHeads * sectorsPerTrack)) /
            sectorsPerTrack;
        p->endSector = (endLbn % sectorsPerTrack) + 1;
    }
    else {
        // Too big flag, c = 1023, h = 254, s = 63
        p->endCylinderHigh = 3;
        p->endCylinderLow = 255;
        p->endHead = 254;
        p->endSector = 63;
    }
    p->firstSector = relSector;
    p->totalSectors = partSize;
    if (!card.writeBlock(0, cache.data))
        return false;

    //  strings needed in file system structures
    char noName[] = "NO NAME    ";
    char fat16str[] = "FAT16   ";

    // write the Master Boot Record
    memset(&cache, 0, sizeof(cache));
    cache.mbr.mbrSig0 = BOOTSIG0;
    cache.mbr.mbrSig1 = BOOTSIG1;
    fat_boot_t* pb = &cache.fbs;
    pb->jump[0] = 0XEB;
    pb->jump[1] = 0X00;
    pb->jump[2] = 0X90;
    for (uint8_t i = 0; i < sizeof(pb->oemId); i++) {
        pb->oemId[i] = ' ';
    }
    pb->bytesPerSector = 512;
    pb->sectorsPerCluster = sectorsPerCluster;
    pb->reservedSectorCount = reservedSectors;
    pb->fatCount = 2;
    pb->rootDirEntryCount = 512;
    pb->mediaType = 0XF8;
    pb->sectorsPerFat16 = fatSize;
    pb->sectorsPerTrack = sectorsPerTrack;
    pb->headCount = numberOfHeads;
    pb->hidddenSectors = relSector;
    pb->totalSectors32 = partSize;
    pb->driveNumber = 0X80;
    pb->bootSignature = EXTENDED_BOOT_SIG;
    pb->volumeSerialNumber = (cardSizeBlocks << 8) + micros();
    memcpy(pb->volumeLabel, noName, sizeof(pb->volumeLabel));
    memcpy(pb->fileSystemType, fat16str, sizeof(pb->fileSystemType));

    // write partition boot sector
    if (!card.writeBlock(relSector, cache.data))
        return false;

    // clear FAT and root directory
    uint32_t count = dataStart - fatStart;
    memset(&cache, 0, sizeof(cache));
    if (!card.writeStart(fatStart, count)) {
        return false;
    }
    for (uint32_t i = 0; i < count; i++) {
        if (!card.writeData(cache.data))
            return false;
    }
    if (!card.writeStop())
        return false;
    memset(&cache, 0, sizeof(cache));
    cache.fat16[0] = 0XFFF8;
    cache.fat16[1] = 0XFFFF;

    // write first block of FAT and backup for reserved clusters
    if (!card.writeBlock(fatStart, cache.data)
    || !card.writeBlock(fatStart + fatSize, cache.data)) {
        return false;
    }

    Serial.println(F("Format done"));
    return true;
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
/** Read the pusher key/secret/channel sent by the Companion */
void Configuration::readPusher(char* buffer, uint8_t bufferSize) {
    JsonStream json = JsonStream(buffer, bufferSize);

    char key[22] = {0};
    char secret[22] = {0};
    char channel[22] = {0};

    json.getStringByName_P(WIZARD_KEY_KEY, key, 22);
    json.getStringByName_P(WIZARD_KEY_SECRET, secret, 22);
    json.getStringByName_P(WIZARD_KEY_CHANNEL, channel, 22);

    if (strcmp(key_, key) != 0
    || strcmp(secret_, secret) != 0
    || strcmp(channel_, channel) != 0) {
        key_ = key;
        secret_ = secret;
        channel_ = channel;
        savePusher();
    }
}
//------------------------------------------------------------------------------
/** Read the username and password sent by the Companion */
void Configuration::readUserAndPass(char* buffer, uint8_t bufferSize) {
    JsonStream json = JsonStream(buffer, bufferSize);

    char newUsername[64] = {0};
    char newPassword[128] = {0};

    json.getStringByName_P(WIZARD_KEY_USER, newUsername, 64);
    json.getStringByName_P(WIZARD_KEY_PASSWORD, newPassword, 128);

    if (strcmp(username_, newUsername) != 0
    || strcmp(password_, newPassword) != 0) {
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
    bool dhcp = (strcmp_P(mode, WIZARD_DHCP) == 0);
    if (ssid == NULL || passphrase == NULL)
        return;
    if (dhcp == false && (ip == NULL || mask == NULL || gateway == NULL))
        return;

    // update the configuration of the Wi-Fi module
    if (dhcp == true) {
        wifly_->setWlanConfig(ssid, passphrase);
    }
    else {
        wifly_->setWlanConfig(ssid, passphrase, ip, mask, gateway);
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
    char buffer[512] = {0};
    write_P(WIZARD_STARTUP);

    uint32_t deadline = millis() + timeout;
    while (millis() < deadline) {
        memset(buffer, 0x00, 512);
        int nbBytes = readBytesUntil(COMMAND_END_CHAR, buffer, 512);
        JsonStream json = JsonStream(buffer, nbBytes);

        char cmd[16] = {0};
        int nBytes = json.getStringByName_P(COMMAND_TYPE, cmd, 16);

        if (nBytes > 0) {

            if (strcmp_P(cmd, COMMAND_DEVICEID) == 0) {
                sendDeviceId();
                deadline = millis() + timeout;
            }
            else if (strcmp_P(cmd, COMMAND_AUTH) == 0) {
                DEBUG_LOG("Command received: set authentication.");
                readUserAndPass(buffer, nbBytes);
                deadline = millis() + timeout;
                DEBUG_LOG("Authentication set.");
            }
            else if (strcmp_P(cmd, COMMAND_WLAN) == 0) {
                DEBUG_LOG("Command received: set WLAN config.");
                readWifiSettings(buffer, nbBytes);
                deadline = millis() + timeout;
                DEBUG_LOG("WLAN config set.");
            }
            else if (strcmp_P(cmd, COMMAND_PUSHER) == 0) {
                DEBUG_LOG("Command received: setup Pusher messaging.");
                readPusher(buffer, nbBytes);
                deadline = millis() + timeout;
                DEBUG_LOG("Pusher information set.");
            }
            else if (strcmp_P(cmd, COMMAND_FACTORY) == 0) {
                DEBUG_LOG("Command received: perform factory reset.");
                wifly_->resetConfigToDefault();
                wifly_->getDeviceId(deviceId_);
                saveDeviceId();
                deadline = millis() + timeout;
                DEBUG_LOG("Reset to default config performed.");
            }
            else if (strcmp_P(cmd, COMMAND_UPDATE) == 0) {
                DEBUG_LOG("Command received: update Wi-Fi firmware.");
                wifly_->updateFirmware();
                deadline = millis() + timeout;
                DEBUG_LOG("Wi-Fi firmware updated.");
            }
            else if (strcmp_P(cmd, COMMAND_FORMAT) == 0) {
                DEBUG_LOG("Command received: format SD card.");
                formatSdCard();
                deadline = millis() + timeout;
                DEBUG_LOG("SD card formatted.");
            }
            else if (strcmp_P(cmd, COMMAND_BOOTLOADER) == 0) {
                DEBUG_LOG("Command received: enable Wi-Fi bootloader.");
                enableBootloader();
                deadline = millis() + timeout;
                DEBUG_LOG("Bootloader enabled.");
            }
            else {
                DEBUG_LOG("Command not recognized.");
            }
        }
    }
}
