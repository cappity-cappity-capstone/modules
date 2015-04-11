#ifndef __ALERT_LIFECYCLE_H__
#define __ALERT_LIFECYCLE_H__

#include "application.h"

class AlertLifecycle {
public:
    AlertLifecycle(IPAddress, char*);

    void create(char*, char*);
    bool setTrigger(bool);

    bool isValid;
private:
    void createAlertPath();
    void createTriggerPath();

    IPAddress hostIp;
    char *alertId;
    char *alertPath;
    char *triggerPath;

    bool prevTrigger;
};

#endif
