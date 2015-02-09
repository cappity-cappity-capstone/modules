#include "DeviceStartup.h"

IPAddress ccsIpAddress;

void setup() {
    delay(5000);
    Serial.begin(9600);

    DeviceStartup *ds = new DeviceStartup(10000, "HAL CCS");
    ccsIpAddress = ds->listenForRemoteIP();
    Serial.println(ccsIpAddress);
}

void loop() {
    Serial.print("CCS Ip address: ");
    Serial.println(ccsIpAddress);
    delay(1000);
}
