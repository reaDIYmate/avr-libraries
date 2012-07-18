#include "Gmail.h"

//------------------------------------------------------------------------------
const char STRING_API_GMAIL[] PROGMEM = "gmail/search";
//------------------------------------------------------------------------------

Gmail::Gmail(Api &api, Settings &settings, PGM_P motionName, PGM_P soundName) : 
    api_(&api),
	settings_(&settings),
    count_(-1),
	motionName_(motionName),
	soundName_(soundName)
{
}

bool Gmail::update(){
    int countCurrent;

    api_->call(STRING_API_GMAIL);
    countCurrent = api_->getIntegerByName("count");

    if(countCurrent != count_ && count_ != -1){
        if(countCurrent > count_ ){
            count_ = countCurrent;
            return true;
        }
        else{
            count_ = countCurrent;
        }
    }
    if(count_ == -1){
        count_ = countCurrent;
    }
    return false;
}

char* Gmail::getMotionFilename() {
    return settings_->getByName(motionName_);
}

char* Gmail::getSoundFilename() {
    return settings_->getByName(soundName_);
}
