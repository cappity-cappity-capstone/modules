#ifndef __DEVICE_LIFECYCLE_H__
#define __DEVICE_LIFECYCLE_H__

#include "application.h"

class DeviceLifecycle {
public:
    DeviceLifecycle(IPAddress, char*);

    void create(char*, char*);
    bool getState(float&);

    bool isValid;
private:
    void createDevicePath();
    void createStatePath();

    IPAddress hostIp;
    char *deviceId;
    char *devicePath;
    char *statePath;

    float prevState;
};

#endif
