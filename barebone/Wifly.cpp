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
#include <Wifly.h>
//------------------------------------------------------------------------------
// Numeric constants
/** Number of attempts to get a command prompt before reset */
uint8_t const MAX_COMMAND_ERRORS = 2;
/** Number of attempts to associate with the access point before reset */
uint8_t const MAX_WLAN_ERRORS = 2;
/** Number of attempts to open a socket to the host before reset */
uint8_t const MAX_SOCKET_ERRORS = 2;
/** Number of critical errors before giving up */
uint8_t const MAX_CRITICAL_ERRORS = 2;
/** Interval to wait between to consecutive attempts (in ms) */
uint32_t const RETRY_INTERVAL = 2000;
/** Timeout for trying to open a TCP socket to a remote host (in ms) */
uint16_t const SOCKET_TIMEOUT = 5000;
/** UART timeout (in ms) */
uint32_t const UART_TIMEOUT = 500;
/** WLAN timeout (in ms) */
uint16_t const WLAN_TIMEOUT = 5000;
/** Special character used for text communications with the WiFly */
uint8_t const WIFLY_REPLACE_CHAR = '\r';
//------------------------------------------------------------------------------
// WiFly commands
/** Exit command mode */
const char PROGMEM WIFLY_EXIT[] = "exit";
/** Command to reset factory */
const char PROGMEM WIFLY_FACTORY_RESET[] = "factory RESET";
/** Command to update firmware */
const char PROGMEM WIFLY_FTP_UPDATE[] = "ftp update\r";
/** Get the WiFly MAC address */
const char PROGMEM WIFLY_GET_MAC_ADDRESS[] = "get mac\r";
/** Exit command mode */
const char PROGMEM WIFLY_JOIN[] = "join\r";
/** Open a TCP socket */
const char PROGMEM WIFLY_OPEN[] = "open ";
/** Save the configuration */
const char PROGMEM WIFLY_SAVE_CONFIG[] = "save";
/** Set the connexion close message */
const char PROGMEM WIFLY_SET_COMM_CLOSE[] = "set comm close";
/** Set the connexion open message */
const char PROGMEM WIFLY_SET_COMM_OPEN[] = "set comm open";
/** Set the connexion remote message */
const char PROGMEM WIFLY_SET_COMM_REMOTE[] = "set comm remote";
/** Set the TCP host */
const char PROGMEM WIFLY_SET_DNS_NAME[] = "set dns name";
/** Set the IP address of the Roving FTP server */
const char PROGMEM WIFLY_SET_FTP_ADDRESS[] = "set ftp address 208.109.14.133";
/** Set the IP address of the WiFly GSX module */
const char PROGMEM WIFLY_SET_IP_ADDRESS[] = "set ip address";
/** Enable/disable DHCP mode */
const char PROGMEM WIFLY_SET_IP_DHCP[] = "set ip dhcp";
/** Setup the IP settings */
const char PROGMEM WIFLY_SET_IP_FLAGS[] = "set ip flags";
/** Set the gateway IP address */
const char PROGMEM WIFLY_SET_IP_GATEWAY[] = "set ip gateway";
/** Set the network mask */
const char PROGMEM WIFLY_SET_IP_NETMASK[] = "set ip netmask";
/** Set the TCP port */
const char PROGMEM WIFLY_SET_IP_REMOTE[] = "set ip remote";
/** Set the TCP host */
const char PROGMEM WIFLY_SET_IP_TCPMODE[] = "set ip tcp-mode";
/** Set the replace character */
const char PROGMEM WIFLY_SET_OPT_REPLACE[] = "set opt replace";
/** Set the I/O functionality */
const char PROGMEM WIFLY_SET_SYS_IOFUNC[] = "set sys iofunc";
/** Set the UART mode */
const char PROGMEM WIFLY_SET_UART_MODE[] = "set uart mode";
/** Set the UART baudrate value */
const char PROGMEM WIFLY_SET_UART_RAW[] = "set uart raw";
/** Set the WLAN join mode */
const char PROGMEM WIFLY_SET_WLAN_JOIN[] = "set wlan join";
/** Set the link monitor interval */
const char PROGMEM WIFLY_SET_WLAN_LINKMON[] = "set wlan linkmon";
/** Set the passphrase for WPA and WPA2 security modes */
const char PROGMEM WIFLY_SET_WLAN_PHRASE[] = "set w phrase";
/** Set the SSID to associate with - whitespaces must be replaced with '$' */
const char PROGMEM WIFLY_SET_WLAN_SSID[] = "set w ssid";
//------------------------------------------------------------------------------
// WiFly messages
/** Command successfully executed */
const char PROGMEM WIFLY_AOK[] = "AOK";
/** Command mode successfully entered */
const char PROGMEM WIFLY_CMD[] = "CMD";
/** Saved configuration in memory */
const char PROGMEM WIFLY_CONFIG_SAVED[] = "Storing in config";
/** Enter command mode */
const char PROGMEM WIFLY_ENTER_COMMAND[] = "$$$";
/** Left command mode */
const char PROGMEM WIFLY_EXITED[] = "EXIT";
/** Set factory defaults */
const char PROGMEM WIFLY_FACTORY_MESSAGE[] = "Set Factory Defaults";
/** FTP update successfully executed */
const char PROGMEM WIFLY_UPDATE_OK[] = "UPDATE OK";
//------------------------------------------------------------------------------
/**
 * Construct an instance of SerialStream.
 *
 * \param[in] serial The serial port that will send and receive data.
 * \param[in] resetPin The AVR pin connected to RST.
 * \param[in] gpio4Pin The AVR pin connected to GPIO4.
 * \param[in] gpio5Pin The AVR pin connected to GPIO5.
 * \param[in] gpio6Pin The AVR pin connected to GPIO6.
 */
Wifly::Wifly(HardwareSerial &serial, uint8_t resetPin, uint8_t gpio4Pin,
    uint8_t gpio5Pin, uint8_t gpio6Pin) :
    SerialStream(serial, UART_TIMEOUT),
    resetPin_(resetPin),
    gpio4Pin_(gpio4Pin),
    gpio5Pin_(gpio5Pin),
    gpio6Pin_(gpio6Pin)
{
}
//------------------------------------------------------------------------------
/**
 * GPIO4 goes high once the module is associated.
 *
 * \return true is returned if the module is currently on a WLAN.
 *
 * \note GPIO4 goes high if the module is associated with an access point.
 */
bool Wifly::associated() {
    return digitalReadFast(gpio4Pin_);
}
//------------------------------------------------------------------------------
/** Check if the WiFly is connected to the access point.
 *
 * \param[in] timeout The time limit for checking the association status.
 *
 * \note GPIO4 goes high if the module is associated with an access point.
 */
bool Wifly::associated(uint16_t timeout) {
    uint32_t start = millis();
    while (millis() < start + timeout) {
        if (associated())
            return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/**
 * Wait for a response from the host.
 *
 * \return true if the host responded, false if it timed out.
 */
bool Wifly::awaitResponse() {
    uint32_t start = millis();
    while (millis() < start + SOCKET_TIMEOUT) {
        if (connected() && available()) {
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
/** Force the WiFly to close the TCP connection. */
void Wifly::closeSocket() {
    digitalWriteFast(gpio5Pin_, LOW);
}
//------------------------------------------------------------------------------
/**
 * Open a socket to the host.
 *
 * \return true is returned for success and false is returned for failure.
 *
 * \param[in] host The domain name or the IP address of the host.
 *
 * \return true is returned for success and false is returned for failure.
 */
bool Wifly::connect(const char* host) {
    enum state {
        RESETTING,
        ENTERING_COMMAND_MODE,
        SETTING_HOST,
        JOINING_WLAN,
        OPENING_SOCKET,
        CRITICAL_ERROR
    } state = OPENING_SOCKET;

    struct errors {
        uint8_t wlan;
        uint8_t socket;
        uint8_t critical;
    } errors = {0, 0, 0};

    do {
        switch (state) {
            case RESETTING :
#ifdef DEBUG
                Serial.println(F("RESETTING"));
#endif
                reset();
                state = ENTERING_COMMAND_MODE;
                break;
            case ENTERING_COMMAND_MODE :
#ifdef DEBUG
                Serial.println(F("ENTERING_COMMAND_MODE"));
#endif
                if (enterCommandMode())
                    state = SETTING_HOST;
                else
                    state = CRITICAL_ERROR;
                break;
            case SETTING_HOST :
#ifdef DEBUG
                Serial.println(F("SETTING_HOST"));
#endif
                if (setHost(host)) {
                    state = JOINING_WLAN;
                    memset(host_, 0x00, WIFLY_HOST_BUFFER_SIZE);
                    strlcpy(host_, host, WIFLY_HOST_BUFFER_SIZE);
                }
                else
                    state = CRITICAL_ERROR;
                break;
            case JOINING_WLAN :
#ifdef DEBUG
                Serial.println(F("JOINING_WLAN"));
#endif
                if (!associated())
                    join();
                if (associated(WLAN_TIMEOUT))
                    state = OPENING_SOCKET;
                else if (++errors.wlan == MAX_WLAN_ERRORS)
                    state = CRITICAL_ERROR;
                else
                    delay(RETRY_INTERVAL);
                break;
            case OPENING_SOCKET :
#ifdef DEBUG
                Serial.println(F("OPENING_SOCKET"));
#endif
                if (!associated()) {
                    state = RESETTING;
                    break;
                }
                else if (!connected()) {
                    openSocket();
                }

                if (connected(SOCKET_TIMEOUT)) {
                    if (strcmp(host, host_) != 0) {
                        state = RESETTING;
                        break;
                    }
                    return true;
                }
                else {
                    // make sure the connection is closed before retrying
                    disconnect();
                    if (++errors.socket == MAX_SOCKET_ERRORS) {
                        state = CRITICAL_ERROR;
                    }
                }
                break;
            case CRITICAL_ERROR :
#ifdef DEBUG
                Serial.println(F("CRITICAL_ERROR"));
#endif
                if (errors.critical == MAX_CRITICAL_ERRORS) {
                    disconnect();
                    return false;
                }
                else {
                    ++errors.critical;
                    errors.wlan = 0;
                    errors.socket = 0;
                    state = RESETTING;
                }
                break;
            default :
                break;
        }
    } while (1);
}
//------------------------------------------------------------------------------
/**
 * Check whether the module has an open TCP socket.
 *
 * \return true is returned if the module currently has an open socket.
 *
 * \note GPIO6 goes high if the connection to the remote host is successful.
 */
bool Wifly::connected() {
    return digitalReadFast(gpio6Pin_);
}
//------------------------------------------------------------------------------
/**
 * Check whether the module has an open TCP socket.
 *
 * \param[in] timeout The time limit for checking the connection status.
 *
 * \return true is returned if the module has an open socket before the timeout.
 */
bool Wifly::connected(uint16_t timeout) {
    uint32_t start = millis();
    while (millis() < start + timeout) {
        if (connected()) {
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
/**
 * Check if the module is connected to a given host.
 *
 * \return true is returned if the module is currently connected to the host.
 */
bool Wifly::connectedTo_P(PGM_P host) {
    if (!connected())
        return false;

    return (strcmp_P(host_, host) == 0);
}
//------------------------------------------------------------------------------
/** Disconnect from host - close TCP socket. */
void Wifly::disconnect() {
    while (connected())
        closeSocket();
}
//------------------------------------------------------------------------------
/**
 * Get the command prompt on the WiFly module.
 *
 * \return true is returned if command mode is successfully entered.
 */
bool Wifly::enterCommandMode() {
    uint8_t attemptCount = 0;
    do {
        // there is a 250ms buffer before the escape sequence
        delay(250);
        // request a new command prompt
        write_P(WIFLY_ENTER_COMMAND);
        flush();
        // look for the command string
        if (find_P(WIFLY_CMD))
            return true;
        // if the error threshold has been reached, give up
        if (++attemptCount == MAX_COMMAND_ERRORS) {
            return false;
        }
    } while (1);
}
//------------------------------------------------------------------------------
/**
 * Send a command to the WiFly and wait for the expected response.
 *
 * \param[in] command The WiFLy command to execute.
 * \param[in] expectedReturn The answer expected from the WiFly module.
 * \param[in] parameter A parameter passed with the command.
 *
 * \return true is returned if the command is send and the expected answer is
 * successfully returned.
 */
bool Wifly::executeCommand(PGM_P command, PGM_P expectedReturn,
    const char* parameter) {
    write_P(command);
    if (parameter != NULL) {
        write(' ');
        print(parameter);
    }
    write('\r');
    return find_P(expectedReturn);
}
//------------------------------------------------------------------------------
/**
 * Send a command to the WiFly and wait for the expected response.
 *
 * \param[in] command The WiFLy command to execute.
 * \param[in] expectedReturn The answer expected from the WiFly module.
 *
 * \return true is returned if the command is send and the expected answer is
 * successfully returned.
 */
bool Wifly::executeCommand(PGM_P command, PGM_P expectedReturn,
    long parameter) {
    write_P(command);
    write(' ');
    print(parameter);
    write('\r');
    return find_P(expectedReturn);
}
//------------------------------------------------------------------------------
/**
 * Get the WiFly MAC address.
 *
 * \param[out] output The location the device ID will be written to.
 */
void Wifly::getDeviceId(char* output) {
    reset();
    while(!enterCommandMode());
    write_P(WIFLY_GET_MAC_ADDRESS);

    char buffer[30] = {0};
    int nBytes = readBytes(buffer, 29);
    BufferedStream bufferedStream = BufferedStream(buffer + 2, nBytes);

    bufferedStream.find('=');
    char mac[18];
    bufferedStream.readBytesUntil('\r', mac, 18);

    int n = 0;
    for (int i = 2; i <= 10; i += 2){
        mac[i] = mac[i + 1 + n];
        mac[i + 1] = mac[i + 2 + n];
        n++;
    }

    memcpy((void*)output, (void*)mac, 12);
    reset();
}
//------------------------------------------------------------------------------
/** Initialize the WiFly module. */
void Wifly::initialize() {
    begin(115200);
    pinModeFast(resetPin_, OUTPUT);
    pinModeFast(gpio4Pin_, INPUT);
    pinModeFast(gpio5Pin_, OUTPUT);
    pinModeFast(gpio6Pin_, INPUT);
}
//------------------------------------------------------------------------------
/* Command the WiFly to join the WLAN stored in memory. */
void Wifly::join() {
    write_P(WIFLY_JOIN);
}
//------------------------------------------------------------------------------
/**
 * Force the Wifly to keep the TCP socket open - the module will re-open a
 * TCP connection to the most recent host when GPIO5 is driven high.
 */
void Wifly::openSocket() {
    digitalWriteFast(gpio5Pin_, HIGH);
}
//------------------------------------------------------------------------------
/** Perform a hardware reset of the WiFly module. */
void Wifly::reset() {
    digitalWriteFast(resetPin_, LOW);
    delay(1);
    digitalWriteFast(resetPin_, HIGH);
    // total boot time is 150ms
    delay(150);
}
//------------------------------------------------------------------------------
/**
 * Setup the WiFly module to connect to a given access point.
 *
 * \param[in] ssid The WLAN SSID.
 * \param[in] passphrase The security passphrase of the WLAN.
 * \param[in] ip The IP address affected to the WiFly module (optional).
 * \param[in] mask The network mask to use (optional).
 * \param[in] gateway The IP address of the local gateway (optional).
 *
 * \note The IP address, subnet mask and gateway address will be overriden when
 * using DHCP.
 */
void Wifly::setConfig(const char* ssid, const char* passphrase, const char* ip,
    const char* mask, const char* gateway) {
    reset();
    while(!enterCommandMode());

    // setup the access point SSID and security phrase:
    executeCommand(WIFLY_SET_WLAN_SSID, WIFLY_AOK, ssid);
    executeCommand(WIFLY_SET_WLAN_PHRASE, WIFLY_AOK, passphrase);

    // setup static IP, gateway and netmask
    if (ip != NULL && mask != NULL && gateway != NULL) {
        executeCommand(WIFLY_SET_IP_DHCP, WIFLY_AOK, 0L);
        executeCommand(WIFLY_SET_IP_ADDRESS, WIFLY_AOK, ip);
        executeCommand(WIFLY_SET_IP_GATEWAY, WIFLY_AOK, gateway);
        executeCommand(WIFLY_SET_IP_NETMASK, WIFLY_AOK, mask);
    }
    // enable DHCP in cache mode
    else {
        executeCommand(WIFLY_SET_IP_DHCP, WIFLY_AOK, 3);
    }

    executeCommand(WIFLY_SAVE_CONFIG, WIFLY_CONFIG_SAVED);
    executeCommand(WIFLY_EXIT, WIFLY_EXITED);
    reset();
}
//------------------------------------------------------------------------------
/** Setup the RN171. */
void Wifly::setFirstConfig() {
    reset();
    while(!enterCommandMode());

    // disable echo of RX data and replace the version string with a single CR
    executeCommand(WIFLY_SET_UART_MODE, WIFLY_AOK, 0x21);
    executeCommand(WIFLY_SET_OPT_REPLACE, WIFLY_AOK, WIFLY_REPLACE_CHAR);

    // enable the link monitor threshold with the recommended value
    executeCommand(WIFLY_SET_WLAN_LINKMON, WIFLY_AOK, 5);

    // disable access point autojoin
    executeCommand(WIFLY_SET_WLAN_JOIN, WIFLY_AOK, 0L);

    // disable the socket monitor strings
    executeCommand(WIFLY_SET_COMM_OPEN, WIFLY_AOK, 0L);
    executeCommand(WIFLY_SET_COMM_CLOSE, WIFLY_AOK, 0L);
    executeCommand(WIFLY_SET_COMM_REMOTE, WIFLY_AOK, 0L);

    // close any open TCP connection when the WLAN link is lost
    executeCommand(WIFLY_SET_IP_FLAGS, WIFLY_AOK, 0x06);

    // force DNS
    executeCommand(WIFLY_SET_IP_TCPMODE, WIFLY_AOK, 0x04);

    // enable the alternate functions of the LEDs
    executeCommand(WIFLY_SET_SYS_IOFUNC, WIFLY_AOK, 0x70);

    executeCommand(WIFLY_SAVE_CONFIG, WIFLY_CONFIG_SAVED);
    executeCommand(WIFLY_EXIT, WIFLY_EXITED);
    reset();
}
//------------------------------------------------------------------------------
/**
 * Set the TCP host.
 *
 * \param[in] host A string representing the IP address or the DNS name of the
 * host.
 *
 * \return true is returned if the host is successfully set, false is returned
 * if the command fails.
 */
bool Wifly::setHost(const char* host) {
    if (!executeCommand(WIFLY_SET_IP_REMOTE, WIFLY_AOK, 80))
        return false;
    else if (!executeCommand(WIFLY_SET_DNS_NAME, WIFLY_AOK, host))
        return false;
    else
        return true;
}
//------------------------------------------------------------------------------
/** Update the RN171 firmware to the latest version. */
void Wifly::updateFirmware() {
    reset();
    while(!enterCommandMode());

    join();

    // set ftp address
    executeCommand(WIFLY_SET_FTP_ADDRESS, WIFLY_AOK);
    executeCommand(WIFLY_SAVE_CONFIG, WIFLY_CONFIG_SAVED);

    // ftp update
    write_P(WIFLY_FTP_UPDATE);
    while(!find_P(WIFLY_UPDATE_OK));

    // factory reset
    executeCommand(WIFLY_FACTORY_RESET, WIFLY_FACTORY_MESSAGE);

    executeCommand(WIFLY_SAVE_CONFIG, WIFLY_CONFIG_SAVED);
    executeCommand(WIFLY_EXIT, WIFLY_EXITED);

    reset();
}
