#include "DeviceStartup.h"
#include "DeviceLifecycle.h"

IPAddress ccsIpAddress;
DeviceLifecycle *dl1;

void setup() {
    delay(5000);
    Serial.begin(9600);

    DeviceStartup *ds = new DeviceStartup(10000, "HAL CCS");
    ccsIpAddress = ds->listenForRemoteIP();
    Serial.println(ccsIpAddress);

    dl1 = new DeviceLifecycle(ccsIpAddress, "lock1");
}

void loop() {
    Serial.print("CCS Ip address: ");
    Serial.println(ccsIpAddress);
    Serial.print("Watchdog: ");
    Serial.println(dl1->performWatchdog());
    delay(2000);
}
