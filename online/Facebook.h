#ifndef FACEBOOK_H
#define FACEBOOK_H

#include <Api.h>
#include <Settings.h>

class Facebook {

public:
    Facebook(Api &api, Settings &settings, PGM_P motionName, PGM_P soundName);

    bool update();
	char* getMotionFilename();
	char* getSoundFilename();

private:
    Api *api_;
	Settings *settings_;
    int count_;
	PGM_P motionName_;
	PGM_P soundName_;
};

#endif // FACEBOOK_H
