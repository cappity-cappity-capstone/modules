#ifndef __DEVICE_LIFECYCLE_H__
#define __DEVICE_LIFECYCLE_H__

#include "application.h"

class DeviceLifecycle {
public:
    DeviceLifecycle(IPAddress, char*);

    bool performWatchdog();
    bool getStatus(float *);
    bool setStatus(float);
private:
    IPAddress hostIp;
    char *deviceId;
    char *watchdogPath;
};

#endif
