#include "DeviceStartup.h"
#include "DeviceLifecycle.h"

IPAddress ccsIpAddress;
DeviceLifecycle *dl1;
unsigned int nextTime = 0;

void setup() {
    Serial.begin(9600);

    DeviceStartup *ds = new DeviceStartup(10000, "HAL CCS");
    ccsIpAddress = ds->listenForRemoteIP();
    Serial.println(ccsIpAddress);

    int pings = WiFi.ping(ccsIpAddress);
    Serial.print("Pings received: ");
    Serial.println(pings);

    dl1 = new DeviceLifecycle(ccsIpAddress, "lock1");
}

void performWatchdog(void) {
    if (WiFi.ready()) {
        Serial.print("CCS Ip address: ");
        Serial.println(ccsIpAddress);
        if (dl1->performWatchdog()) {
            Serial.print("Watchdog: ");
            Serial.println("Valid");
        } else {
            Serial.print("Watchdog: ");
            Serial.println("Invalid");
        }
    }
}

void performStatusCheck(void) {
    static float state;
    Serial.print("Pinging IP: ");
    Serial.println(ccsIpAddress);
    if (WiFi.ready()) {
        if (dl1->getState(state)) {
            Serial.print("Status: ");
            Serial.println(state);
        } else {
            Serial.print("Status: ");
            Serial.println("Could not connect");
        }
    }
}

void loop(void) {
    Serial.println("Loop");

    if (nextTime > millis()) {
        Spark.process();
        return;
    }

    performStatusCheck();

    nextTime = millis() + 5000;
}
