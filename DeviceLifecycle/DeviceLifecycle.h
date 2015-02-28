#ifndef __DEVICE_LIFECYCLE_H__
#define __DEVICE_LIFECYCLE_H__

#include "application.h"

class DeviceLifecycle {
public:
    DeviceLifecycle(IPAddress, char*);

    bool performWatchdog();

    void create(char*, char*);
    bool getState(float&);
    bool setState(float);

    bool isValid;
private:
    void createDevicePath();
    void createWatchdogPath();
    void createStatePath();

    IPAddress hostIp;
    char *deviceId;
    char *devicePath;
    char *watchdogPath;
    char *statePath;

    float prevState;
};

#endif
