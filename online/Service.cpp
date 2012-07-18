#include "Service.h"

Service::Service(Settings &settings) : 
    settings_(&settings)
{
}

char* Service::getMotionFilename(PGM_P motionName) {
    return settings_->getByName(motionName);
}

char* Service::getSoundFilename(PGM_P soundName) {
    return settings_->getByName(soundName);
}
