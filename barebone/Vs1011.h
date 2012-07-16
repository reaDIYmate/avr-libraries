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
#ifndef VS1011_H
#define VS1011_H
/**
 * \file
 * \brief Vs1011 class for the VLSI VS1011 MP3 decoder chip.
 */
#include <digitalWriteFast.h>
//------------------------------------------------------------------------------
// VS1011 commands
/** READ - read a single 16-bit register */
uint8_t const VS1011_READ_COMMAND = 0x03;
/** WRITE - write a single 16-bit register */
uint8_t const VS1011_WRITE_COMMAND = 0x02;
//------------------------------------------------------------------------------
// VS1011 Serial Control Interface (SCI) registers.
/** MODE - mode control register */
uint8_t const VS1011_SCI_MODE_REGISTER = 0x00;
/** CLOCKF - clock frequency and multiplier */
uint8_t const VS1011_SCI_CLOCKF_REGISTER = 0x03;
/** VOL - volume control register */
uint8_t const VS1011_SCI_VOL_REGISTER = 0x0B;
//------------------------------------------------------------------------------
// Flags for VS1011 initialization modes
/** initialize() modeFlag for regular playback mode */
uint8_t const VS1011_REGULAR_MODE = 0x00;
/** initialize() modeFlag for sine test mode */
uint8_t const VS1011_SINE_TEST_MODE = 0x01;
//------------------------------------------------------------------------------
// 8-byte sine test control sequences
/** Initialization sequence */
uint8_t const VS1011_SINE_TEST_START[8] = {
	0x53, 0xEF, 0x6E, 0x07, 0x00, 0x00, 0x00, 0x00};
/** Termination sequence */
uint8_t const VS1011_SINE_TEST_STOP[8] = {
	0x45, 0x78, 0x69, 0x74, 0x00, 0x00, 0x00, 0x00};
//------------------------------------------------------------------------------
/**
 * \class Vs1011
 * \brief Send commands and data to a VS1011 chip.
 */
class Vs1011 {
public:
	Vs1011(uint8_t dataRequestPin, uint8_t resetPin, uint8_t dataSelectPin,
		uint8_t controlSelectPin);
	~Vs1011();
	void initialize(uint8_t modeFlag = VS1011_REGULAR_MODE);
	void sendBytes(const uint8_t* sourceBuffer, uint16_t nBytes);
	void sendZeros(uint16_t nBytes);
	void setVolume(uint8_t volume);
//------------------------------------------------------------------------------
private:
	/** Activate the Serial Control Interface (SCI). */
	void selectControl() {digitalWriteFast(controlSelectPin_, LOW);}
	/** Force the SCI into standby mode. */
	void deselectControl() {digitalWriteFast(controlSelectPin_, HIGH);}
	/** Activate the Serial Data Interface (SDI). */
	void selectData() {digitalWriteFast(dataSelectPin_, LOW);}
	/** Force the SDI into standby mode. */
	void deselectData() {digitalWriteFast(dataSelectPin_, HIGH);}
	uint16_t readRegister(uint8_t address);
	void writeRegister(uint8_t address, uint8_t highByte, uint8_t lowByte);
	/**
	 * Data request pin (DREQ)
	 * Used to signal if VS1011's FIFO is capable of receiving data.
	 */
	const uint8_t dataRequestPin_;
	/** Hardware reset pin (XRESET) */
	const uint8_t resetPin_;
	/** Active low SDI chip select (XDCS) */
	const uint8_t dataSelectPin_;
	/** Active low SCI chip select (XCS) */
	const uint8_t controlSelectPin_;
};

#endif // VS1011_H
