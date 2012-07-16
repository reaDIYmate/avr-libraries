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
#include <Alerts.h>
//------------------------------------------------------------------------------
/** Path to the API that provides the match counts */
const char METHOD_SERVICES_UPDATE[] PROGMEM = "services/update";
//------------------------------------------------------------------------------
// Keys associated to the match counts in the JSON response of the API
const char PROGMEM MATCH0[] = "match_0";
const char PROGMEM MATCH1[] = "match_1";
const char PROGMEM MATCH2[] = "match_2";
const char PROGMEM MATCH3[] = "match_3";
const char PROGMEM MATCH4[] = "match_4";
const char PROGMEM MATCH5[] = "match_5";
const char PROGMEM MATCH6[] = "match_6";
const char PROGMEM MATCH7[] = "match_7";
/** Flash string array */
const char* matchKeys[] =
{
    MATCH0,
    MATCH1,
    MATCH2,
    MATCH3,
    MATCH4,
    MATCH5,
    MATCH6,
    MATCH7
};
//------------------------------------------------------------------------------
/**
 * Construct an instance of Alerts.
 *
 * \param[in] api The instance of Api to use for API calls.
 * \param[in] settings Instance of Settings used to retrieve the rules.
 */
Alerts::Alerts(Api &api, Settings &settings) :
    index_(0),
    api_(&api),
    settings_(&settings)
{
}
//------------------------------------------------------------------------------
/**
 * Fetch the alert levels from the API.
 *
 * \return The number of values fetched is returned.
 */
uint8_t Alerts::fetch() {
    api_->call(METHOD_SERVICES_UPDATE);
    // two settings are stored for each rule
    return settings_->getNbSettings()/2;
}
//------------------------------------------------------------------------------
/**
 * Get the name of the MP3 file associated to the current rule.
 *
 * \return A pointer to the file name is returned.
 */
char* Alerts::getFilename() {
    return settings_->getSettingByNumber(2*index_+1);
}
//------------------------------------------------------------------------------
/**
 * Check whether the repeat mode is enabled for a given rule.
 *
 * \param[in] i The index of the rule to check.
 *
 * \return true is returned if the provided rule has the repeat mode enabled.
 */
bool Alerts::repeatEnabled(uint8_t i) {
    index_ = i;
    return (strcmp(settings_->getSettingByNumber(2*i), "1") == 0);
}
//------------------------------------------------------------------------------
/**
 * Read the number of matches for a given rule from the API JSON response.
 *
 * \param[in] i The index of the rule to check.
 *
 * \return The number of matches for the provided rule is returned.
 */
int Alerts::readNewValue(uint8_t i) {
    index_ = i;
    return api_->getIntegerByName_P(matchKeys[i]);
}
//------------------------------------------------------------------------------
/**
 * Determine whether an alert should be generated for a given rule.
 *
 * \param[in] i The index of the rule to check.
 *
 * \return true is returned if the provided rule has an alert.
 */
bool Alerts::isActive(uint8_t i) {
    index_ = i;

    int level = readNewValue(i);
    if (level < 0) {
        return false;
    }
    else {
        bool repeatMode = repeatEnabled(i);
        bool hasIncreased = (level > matchValues_[i]);
        bool hasAlert = hasIncreased || (repeatMode && level > 0);
        matchValues_[i] = level;
        return hasAlert;
    }
}
//------------------------------------------------------------------------------
/**
 * Fetch the match counts from the API and store them.
 *
 * \note This method should be called during initialization so that the match
 * counts are initialized with correct values.
 */
void Alerts::synchronize() {
    uint8_t nbRules = fetch();
    for (uint8_t i = 0; i < nbRules; i++)
        matchValues_[i] = readNewValue(i);
}
