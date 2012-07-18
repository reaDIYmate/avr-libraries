#include "Facebook.h"

//------------------------------------------------------------------------------
const char STRING_API_FACEBOOK[] PROGMEM = "facebook/update";
//------------------------------------------------------------------------------

Facebook::Facebook(Api &api, Settings &settings, PGM_P motionName, PGM_P soundName) : 
    api_(&api),
	settings_(&settings),
    count_(-1),
	motionName_(motionName),
	soundName_(soundName)
{
}

bool Facebook::update(){
    int countCurrent;
    int pokes;
    int notifications;
    int friendRequests;

    api_->call(STRING_API_FACEBOOK);
    pokes = api_->getIntegerByName("pokes");
    notifications = api_->getIntegerByName("notifications");
    friendRequests = api_->getIntegerByName("friendrequests");

    countCurrent = pokes + notifications + friendRequests;

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

char* Facebook::getMotionFilename() {
    return settings_->getByName(motionName_);
}

char* Facebook::getSoundFilename() {
    return settings_->getByName(soundName_);
}
