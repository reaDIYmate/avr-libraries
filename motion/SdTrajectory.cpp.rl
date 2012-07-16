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
#include <SdTrajectory.h>
//------------------------------------------------------------------------------
//#define DEBUG
#define MAX_OUTPUT 255
#define MIN_OUTPUT 0
//------------------------------------------------------------------------------
// Declare the parser and emit constant static data needed by the Ragel machine.
%%{
  machine parser;
  write data;
}%%
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
    %%{
        action command_start {
            duration = 0;
            destination = 0;
        }

        action duration_add_digit {
            duration = duration * 10 + (*p - '0');
        }

        action destination_add_digit {
            destination = destination * 10 + (*p - '0');
        }

        action destination_add_offset {
            destination = pos + destination;
            if (destination > MAX_OUTPUT)
                destination = MAX_OUTPUT;
        }

        action destination_subtract_offset {
            destination = pos - destination;
            if (destination < MIN_OUTPUT)
                destination = MIN_OUTPUT;
        }

        duration = ([0-9] @duration_add_digit)+;

        absolute = ([0-9] @destination_add_digit)+;
        relative = ('+' absolute %destination_add_offset | '-' absolute
            %destination_subtract_offset);
        destination = (absolute | relative);

        channel = ('SERVO');
        white = [ \t]+;
        eol = ([ \t\r]* '\n');
        command = duration white channel white destination eol?;

        main := (command >command_start);

        write init;
        write exec;
    }%%

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
