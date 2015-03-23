#ifndef __DEVICE_LIFECYCLE_H__
#define __DEVICE_LIFECYCLE_H__

#include "application.h"

unsigned char compareState(float a, float b);

class DeviceLifecycle {
public:
    DeviceLifecycle(IPAddress, const char*);

    bool performWatchdog();

    bool create(char*, char*, IPAddress);
    bool update(char*, char*, IPAddress);
    bool getState(float&);
    bool setState(float);

    bool isValid;

    void setPrevState(float state);
    float getPrevState();

    const char *getDeviceId();
private:
    void setCreateDevicePath();
    void setUpdateDevicePath();
    void setWatchdogPath();
    void setStatePath();
    void generateDeviceRequestBody(char*, char*, char*, char*);
    void generateIp(char*, IPAddress);

    IPAddress hostIp;
    const char *deviceId;
    char *createDevicePath;
    char *updateDevicePath;
    char *watchdogPath;
    char *statePath;

    float prevState;
};

#endif
