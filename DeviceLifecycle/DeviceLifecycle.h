#ifndef __DEVICE_LIFECYCLE_H__
#define __DEVICE_LIFECYCLE_H__

#include "application.h"

class DeviceLifecycle {
public:
    DeviceLifecycle(IPAddress, char*);

    bool performWatchdog();
    bool getState(float&);
    bool setState(float);
private:
    void createWatchdogPath();
    void createStatePath();

    IPAddress hostIp;
    char *deviceId;
    char *watchdogPath;
    char *statePath;

    float state;
};

#endif
