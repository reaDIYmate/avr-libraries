#ifndef TWITTER_H
#define TWITTER_H

#include <Api.h>
#include <Settings.h>

class Twitter {

public:
    Twitter(Api &api, Settings &settings, PGM_P motionName, PGM_P soundName);

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

#endif // TWITTER_H
