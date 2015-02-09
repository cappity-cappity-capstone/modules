#ifndef __MODULE_STARTUP_H__
#define __MODULE_STARTUP_H__

class ModuleStartup {
public:
    ModuleStartup(unsigned int, char *);

    IPAddress listenForRemoteIP();
    IPAddress run();
private:
    unsigned int port;
    char *message;
    UDP socket;
}

#endif
