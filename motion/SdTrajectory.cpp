
/* #line 1 "SdTrajectory.cpp.rl" */
/* reaDIYmate library
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
#include <SdTrajectory.h>
//------------------------------------------------------------------------------
//#define DEBUG
#define MAX_OUTPUT 255
#define MIN_OUTPUT 0
//------------------------------------------------------------------------------
// Declare the parser and emit constant static data needed by the Ragel machine.

/* #line 2 "SdTrajectory.cpp" */
static const int parser_start = 1;
static const int parser_first_final = 13;
static const int parser_error = 0;

static const int parser_en_main = 1;


/* #line 30 "SdTrajectory.cpp.rl" */

//------------------------------------------------------------------------------
/**
 * Construct an instance of SdTrajectory.
 *
 * \param[in] sd A SdFat object.
 * \param[in] sdChipSelectPin The AVR pin connected to CS on the SD card.
 */
SdTrajectory::SdTrajectory(SdFat &sd, uint8_t sdChipSelectPin) :
    sd_(&sd),
    sdChipSelectPin_(sdChipSelectPin)
{
}
//------------------------------------------------------------------------------
/**
* Open a text file containing the description of an animation.
*
* \param[in] filename The file to open.
*
* \return true is returned for success and false is returned for failure.
*/
bool SdTrajectory::begin(const char* filename) {
    if (!sd_->init(SPI_EIGHTH_SPEED, sdChipSelectPin_))
        return false;
    return open(filename, O_READ);
}
//------------------------------------------------------------------------------
/** Stop processing incoming events */
void SdTrajectory::end() {
    close();
}
//------------------------------------------------------------------------------
/**
 * Read the next motion from the choregraphy file.
 *
 * \param[out] dest Location where the motion destination will be written.
 * \param[out] dur Location where the motion duration will be written.
 * \param[in] pos Last known position of the output.
 *
 * \return true is returned if a new line was processed. The value false is
 * returned if no event is available.
 */
bool SdTrajectory::getNextMotion(int* dest, int* dur, uint8_t pos) {
    // Buffer one line of text
    if (readLine(buffer_, sizeof(buffer_)) <= 0)
        return false;

    // Parse buffered line into a motion command
    if (!parseMotion(dest, dur, buffer_, pos))
        return false;

    return (*dur>= 0);
}
//------------------------------------------------------------------------------
/**
 * Compute the next step within the ongoing motion.
 *
 * \param[out] step Location where the step value will be written.
 * \param[out] dur Location where the motion duration will be written.
 * \param[in] pos Last known position of the output.
 * \param[in] dest Target position of the current motion.
 * \param[in] timeLeft Time remaining (in ms) to complete the ongoing motion.
 *
 * \return true is returned if a new step was started.
 */
bool SdTrajectory::getNextStep(int* step, int* dur, int pos, int dest,
    int timeLeft) {
    // don't move if the target position is reached
    int distanceLeft = dest - pos;
    if (distanceLeft == 0) {
        *step = 0;
        *dur = timeLeft;
        return false;
    }
    else {
        // steps are 1° up or 1° down
        *step = (distanceLeft > 0) ? 1 : -1;
        float speed = (float)timeLeft/(float)(distanceLeft);
        *dur = (int)(((float)(*step))*speed);
        return true;
    }
}
//------------------------------------------------------------------------------
/**
 * Parse a command to extract information about the next motion.
 *
 * \param[out] dest Location where the motion destination will be written.
 * \param[out] dur Location where the motion duration will be written.
 * \param[in] data Location of the data to parse.
 * \param[in] pos Last known position of the output.
 *
 * \return true is returned if a new command was parsed. The value false is
 * returned if no valid command is available.
 */
bool SdTrajectory::parseMotion(int* dest, int* dur, const char* data,
    uint8_t pos) {
    // Ragel internal variables
    const char* p = data;
    const char* pe = data + strlen(data);
    const char* eof = pe;
    int cs;

    int destination, duration;

    // Specification of the Ragel parser

/* #line 8 "SdTrajectory.cpp" */
	{
	cs = parser_start;
	}

/* #line 11 "SdTrajectory.cpp" */
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr0;
	goto st0;
st0:
cs = 0;
	goto _out;
tr0:
/* #line 135 "SdTrajectory.cpp.rl" */
	{
            duration = 0;
            destination = 0;
        }
/* #line 140 "SdTrajectory.cpp.rl" */
	{
            duration = duration * 10 + (*p - '0');
        }
	goto st2;
tr3:
/* #line 140 "SdTrajectory.cpp.rl" */
	{
            duration = duration * 10 + (*p - '0');
        }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
/* #line 41 "SdTrajectory.cpp" */
	switch( (*p) ) {
		case 9: goto st3;
		case 32: goto st3;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr3;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	switch( (*p) ) {
		case 9: goto st3;
		case 32: goto st3;
		case 83: goto st4;
	}
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 69 )
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 82 )
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 86 )
		goto st7;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) == 79 )
		goto st8;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 9: goto st9;
		case 32: goto st9;
	}
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	switch( (*p) ) {
		case 9: goto st9;
		case 32: goto st9;
		case 43: goto st10;
		case 45: goto st12;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr12;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr13;
	goto st0;
tr13:
/* #line 144 "SdTrajectory.cpp.rl" */
	{
            destination = destination * 10 + (*p - '0');
        }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
/* #line 124 "SdTrajectory.cpp" */
	switch( (*p) ) {
		case 9: goto tr17;
		case 10: goto tr18;
		case 13: goto tr17;
		case 32: goto tr17;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr13;
	goto st0;
tr17:
/* #line 148 "SdTrajectory.cpp.rl" */
	{
            destination = pos + destination;
            if (destination > MAX_OUTPUT)
                destination = MAX_OUTPUT;
        }
	goto st11;
tr19:
/* #line 154 "SdTrajectory.cpp.rl" */
	{
            destination = pos - destination;
            if (destination < MIN_OUTPUT)
                destination = MIN_OUTPUT;
        }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
/* #line 151 "SdTrajectory.cpp" */
	switch( (*p) ) {
		case 9: goto st11;
		case 10: goto st14;
		case 13: goto st11;
		case 32: goto st11;
	}
	goto st0;
tr18:
/* #line 148 "SdTrajectory.cpp.rl" */
	{
            destination = pos + destination;
            if (destination > MAX_OUTPUT)
                destination = MAX_OUTPUT;
        }
	goto st14;
tr20:
/* #line 154 "SdTrajectory.cpp.rl" */
	{
            destination = pos - destination;
            if (destination < MIN_OUTPUT)
                destination = MIN_OUTPUT;
        }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
/* #line 176 "SdTrajectory.cpp" */
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr16;
	goto st0;
tr16:
/* #line 144 "SdTrajectory.cpp.rl" */
	{
            destination = destination * 10 + (*p - '0');
        }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
/* #line 193 "SdTrajectory.cpp" */
	switch( (*p) ) {
		case 9: goto tr19;
		case 10: goto tr20;
		case 13: goto tr19;
		case 32: goto tr19;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr16;
	goto st0;
tr12:
/* #line 144 "SdTrajectory.cpp.rl" */
	{
            destination = destination * 10 + (*p - '0');
        }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
/* #line 211 "SdTrajectory.cpp" */
	switch( (*p) ) {
		case 9: goto st11;
		case 10: goto st14;
		case 13: goto st11;
		case 32: goto st11;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr12;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof;
	_test_eof3: cs = 3; goto _test_eof;
	_test_eof4: cs = 4; goto _test_eof;
	_test_eof5: cs = 5; goto _test_eof;
	_test_eof6: cs = 6; goto _test_eof;
	_test_eof7: cs = 7; goto _test_eof;
	_test_eof8: cs = 8; goto _test_eof;
	_test_eof9: cs = 9; goto _test_eof;
	_test_eof10: cs = 10; goto _test_eof;
	_test_eof13: cs = 13; goto _test_eof;
	_test_eof11: cs = 11; goto _test_eof;
	_test_eof14: cs = 14; goto _test_eof;
	_test_eof12: cs = 12; goto _test_eof;
	_test_eof15: cs = 15; goto _test_eof;
	_test_eof16: cs = 16; goto _test_eof;

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 13:
/* #line 148 "SdTrajectory.cpp.rl" */
	{
            destination = pos + destination;
            if (destination > MAX_OUTPUT)
                destination = MAX_OUTPUT;
        }
	break;
	case 15:
/* #line 154 "SdTrajectory.cpp.rl" */
	{
            destination = pos - destination;
            if (destination < MIN_OUTPUT)
                destination = MIN_OUTPUT;
        }
	break;
/* #line 255 "SdTrajectory.cpp" */
	}
	}

	_out: {}
	}

/* #line 176 "SdTrajectory.cpp.rl" */


    // Check final state of the parser and choose return value accordingly
    if (cs >= parser_first_final) {
        *dest = destination;
        *dur = duration;
        return true;
    }
    else if (cs == parser_error) {
        Serial.println("Parser error");
        return false;
    }
    else {
        Serial.println("Invalid data");
        return false;
    }
}
//------------------------------------------------------------------------------
/**
 * Read one line from the input file
 *
 * \param[out] buffer Address of the buffer to write data to.
 * \param[in] length Number of bytes to read into the buffer.
 *
 * \return The number of characters placed in the buffer (0 means no valid data
 * found).
 */
int SdTrajectory::readLine(char* buffer, size_t length) {
    int c;
    int index = 0;
    memset(buffer, 0x00, length);
    do {
        c = read();
        if (index > length)
            break;
        else if (c < 0) {
            buffer[index] = 0x00;
            break;
        }
        else {
            buffer[index] = (char)c;
        }
    } while (buffer[index++] != '\n');
    return index;
}
