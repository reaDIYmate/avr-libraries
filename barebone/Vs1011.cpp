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
#include <Vs1011.h>
//------------------------------------------------------------------------------
/** Send a byte over to the SPI bus */
static inline __attribute__((always_inline)) void sendByte(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}
//------------------------------------------------------------------------------
/** Receive a byte from the SPI bus */
static inline __attribute__((always_inline)) uint8_t receiveByte() {
	SPDR = 0xFF;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}
//------------------------------------------------------------------------------
/** Setup the hardware SPI registers of the AVR */
static inline __attribute__((always_inline)) void setupSpiRegisters() {
	// set byte ordering to MSB first
	SPCR &= ~(1 << DORD);
	// set clock polarity and phase for SPI mode 0
	SPCR &= ~((1 << CPOL) | (1 << CPHA));
	// set SPI clock to 2MHz
	SPCR = (SPCR | (1 << SPR0)) & ~(1 << SPR1);
	SPSR |= (1 << SPI2X);
}
//------------------------------------------------------------------------------
/**
 * Construct an instance of Vs1011.
 *
 * \param[in] dataRequestPin The AVR pin connected to DREQ.
 * \param[in] resetPin The AVR pin connected to RESET.
 * \param[in] dataSelectPin The AVR pin connected to XDCS/BSYNC.
 * \param[in] controlSelectPin The AVR pin connected to XCS.
 */
Vs1011::Vs1011(uint8_t dataRequestPin, uint8_t resetPin, uint8_t dataSelectPin,
	uint8_t controlSelectPin) :
	dataRequestPin_(dataRequestPin),
	resetPin_(resetPin),
	dataSelectPin_(dataSelectPin),
	controlSelectPin_(controlSelectPin)
{
}
//------------------------------------------------------------------------------
/** Turn off the chip. */
Vs1011::~Vs1011() {
	digitalWriteFast(resetPin_, LOW);
}
//------------------------------------------------------------------------------
/**
 * Initialize the VS1011 chip
 *
 * \param[in] modeFlag Determines the initialization mode.
 *
 * If \a modeFlag is omitted, the default mode is VS1011_REGULAR_MODE.
 *
 * - VS1011_REGULAR_MODE runs the regular initialization mode.
 * - VS1011_SINE_TEST_MODE runs the sine test mode.
 */
void Vs1011::initialize(uint8_t modeFlag) {
	// configure the VS1011 pins
	pinMode(resetPin_, OUTPUT);
	pinMode(dataRequestPin_, INPUT);
	pinMode(controlSelectPin_, OUTPUT);
	pinMode(dataSelectPin_, OUTPUT);
	deselectControl();
	deselectData();
	// configure the SPI bus lines
	pinMode(SCK, OUTPUT);
	pinMode(MOSI, OUTPUT);
	pinMode(SS, OUTPUT);
	digitalWriteFast(SCK, LOW);
	digitalWriteFast(MOSI, LOW);
	digitalWriteFast(SS, HIGH);

	// turn on the chip
	digitalWriteFast(resetPin_, HIGH);
	delay(1);
	// enable the SPI bus in Master mode
	SPCR |= (1 << SPE) | (1 << MSTR);
	setupSpiRegisters();

	// in test mode, run the VS1011 sine test
	if(modeFlag == VS1011_SINE_TEST_MODE) {
		// enter test mode
		writeRegister(VS1011_SCI_MODE_REGISTER, 0x08, 0x20);
		// send the sine test start header
		selectData();
		for (uint8_t i = 0; i < 8; i++) {
			sendByte(VS1011_SINE_TEST_START[i]);
		}
		deselectData();

		// beep for 500 milliseconds
		delay(500);
		// send the sine test stop header
		selectData();
		for (uint8_t i = 0; i < 8; i++) {
			sendByte(VS1011_SINE_TEST_STOP[i]);
		}
		deselectData();
		// disable the SPI bus
		delay(1);
		SPCR &= ~(1 << SPE);
		// hardware reset
		digitalWriteFast(resetPin_, LOW);
		delay(1);
		digitalWriteFast(resetPin_, HIGH);
		delay(1);
		// enable the SPI bus in Master mode
		SPCR |= (1 << SPE) | (1 << MSTR);
		setupSpiRegisters();
	}

	// trigger software reset
	writeRegister(VS1011_SCI_MODE_REGISTER, 0x08, 0x04);
	delay(1);
	// set the volume to the maximum value
	writeRegister(VS1011_SCI_VOL_REGISTER, 0x00, 0x00);
	delay(1);
	// enable internal clock doubling (external crystal at 12.288MHz)
	writeRegister(VS1011_SCI_CLOCKF_REGISTER, 0x98, 0x00);
	delay(1);
}
//------------------------------------------------------------------------------
/**
 * Send audio data to the MP3 decoder.
 *
 * \param[in] sourceBuffer Pointer to the location of the audio data.
 * \param[in] nBytes Number of bytes to transfer.
 */
void Vs1011::sendBytes(const uint8_t* sourceBuffer, uint16_t nBytes) {
	deselectControl();
	selectData();
	while (digitalReadFast(dataRequestPin_) == LOW);
	for (uint16_t i = 0; i < nBytes; i++) {
		sendByte(sourceBuffer[i]);
	}
	deselectData();
}
//------------------------------------------------------------------------------
/**
 * Send empty data to the MP3 decoder.
 *
 * \param[in] nBytes Number of empty bytes to send.
 */
void Vs1011::sendZeros(uint16_t nBytes) {
	deselectControl();
	selectData();
	for (uint16_t i = 0; i < nBytes; i++) {
		sendByte(0xFF);
	}
	deselectData();
}
//------------------------------------------------------------------------------
/**
 * Change the VS1011 output volume.
 *
 * \param[in] volume A volume level in the range [0-255], with 0 the loudest
 * possible whereas 255 is completely silent.
 */
void Vs1011::setVolume(uint8_t volume) {
	writeRegister(VS1011_SCI_VOL_REGISTER, volume, volume);
	delay(1);
	selectData();
}
//------------------------------------------------------------------------------
/** Read the 16-bit value of a VS1011 register. */
uint16_t Vs1011::readRegister(uint8_t address) {
	setupSpiRegisters();
	deselectData();
	selectControl();
	sendByte(VS1011_READ_COMMAND);
	sendByte(address);
	uint16_t incoming = receiveByte();
	uint16_t result = incoming << 8;
	incoming = receiveByte();
	result |= incoming;
	deselectData();
	return result;
}
//------------------------------------------------------------------------------
/** Write a 16-bit value to a VS1011 register. */
void Vs1011::writeRegister(uint8_t address, uint8_t highByte, uint8_t lowByte) {
	setupSpiRegisters();
	deselectData();
	selectControl();
	sendByte(VS1011_WRITE_COMMAND);
	sendByte(address);
	sendByte(highByte);
	sendByte(lowByte);
	deselectControl();
	while (digitalReadFast(dataRequestPin_) == LOW);
}
