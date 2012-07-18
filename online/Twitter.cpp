#include "Twitter.h"

//------------------------------------------------------------------------------
const char STRING_API_TWITTER[] PROGMEM = "twitter/update";
//------------------------------------------------------------------------------

Twitter::Twitter(Api &api, Settings &settings, PGM_P motionName, PGM_P soundName) : 
    api_(&api),
	settings_(&settings),
    count_(-1),
	motionName_(motionName),
	soundName_(soundName)
{
}

bool Twitter::update(){
    int countCurrent;
	int twitterMentionsCount;
	int twitterFollowers;

    api_->call(STRING_API_TWITTER);
    twitterMentionsCount = api_->getIntegerByName("twitter_mentions_count");
    twitterFollowers = api_->getIntegerByName("twitter_followers");
	
	countCurrent = twitterFollowers + twitterMentionsCount;

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

char* Twitter::getMotionFilename() {
    return settings_->getByName(motionName_);
}

char* Twitter::getSoundFilename() {
    return settings_->getByName(soundName_);
}
