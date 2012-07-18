#ifndef SERVICE_H
#define SERVICE_H

class Service {

public:
    virtual bool update() = 0;
    virtual char* getMotionFilename() = 0;
    virtual char* getSoundFilename() = 0;
};

#endif // SERVICE_H
