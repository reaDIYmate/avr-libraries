/*
 * Cryptosuite - A cryptographic library for Arduino
 * Copyright (C) 2010 Peter Knight (Cathedrow)
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
#ifndef SHA256_H
#define SHA256_H
/**
 * \file
 * \brief Sha256Class
 */
#include <inttypes.h>
#include <Print.h>
//------------------------------------------------------------------------------
/** Size of the key buffer */
#define HASH_LENGTH 32
/** Size of the inner hash buffer */
#define BLOCK_LENGTH 64
//------------------------------------------------------------------------------

class Sha256Class : public Print {
/**
 * \class Sha256Class
 * \brief Compute SHA256 HMAC hashes.
 */
public:
    /** Start computing the HMAC from the input data */
    void initHmac(const uint8_t* secret, int secretLength);
    /** Get the HMAC result from the Sha256 object */
    uint8_t* resultHmac(void);
    /** Write an input byte to the Sha256 object */
    virtual size_t write(uint8_t);
    using Print::write;
//------------------------------------------------------------------------------
private:
    union _buffer {
        uint8_t b[BLOCK_LENGTH];
        uint32_t w[BLOCK_LENGTH/4];
    };
    union _state {
        uint8_t b[HASH_LENGTH];
        uint32_t w[HASH_LENGTH/4];
    };
    void addUncounted(uint8_t data);
    void hashBlock();
    void init(void);
    void pad();
    uint8_t* result(void);
    uint32_t ror32(uint32_t number, uint8_t bits);
    _buffer buffer;
    uint8_t bufferOffset;
    _state state;
    uint32_t byteCount;
    uint8_t keyBuffer[BLOCK_LENGTH];
    uint8_t innerHash[HASH_LENGTH];
};
/** Preinstatiated Sha256 object */
extern Sha256Class Sha256;

#endif // SHA256_H
