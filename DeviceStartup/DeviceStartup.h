#ifndef __DEVICE_STARTUP_H__
#define __DEVICE_STARTUP_H__

#include "application.h"

class DeviceStartup {
public:
    DeviceStartup(unsigned int, char *);

    IPAddress listenForRemoteIP();
    IPAddress run();
private:
    unsigned int port;
    char *message;
    UDP socket;
};

#endif
