#include "DeviceStartup.h"
#include "DeviceLifecycle.h"
#include "SparkIntervalTimer.h"

IPAddress ccsIpAddress;
DeviceLifecycle *dl1;

volatile bool checkStatus = false;
IntervalTimer statusTimer;

float prevState;
float state;

void setup() {
    Serial.begin(9600);

    DeviceStartup *ds = new DeviceStartup(10000, "HAL CCS");
    ccsIpAddress = ds->listenForRemoteIP();
    Serial.println(ccsIpAddress);

    int pings = WiFi.ping(ccsIpAddress);
    Serial.print("Pings received: ");
    Serial.println(pings);

    dl1 = new DeviceLifecycle(ccsIpAddress, "lock1");

    statusTimer.begin(setCheckStatus, 4000, hmSec);
}

void loop(void) {
    Spark.process();

    if (checkStatus) {
        performStatusCheck();
        checkStatus = false;
    }
}

void setCheckStatus(void) {
    Serial.println("Check status!");
    checkStatus = true;
}

void performStatusCheck(void) {
    Serial.print("Pinging IP: ");
    Serial.println(ccsIpAddress);
    if (WiFi.ready()) {
        prevState = state;
        if (dl1->getState(state)) {
            if (compareState(state, prevState) != 0) {
                Serial.print("New state: ");
                Serial.println(state);
                Serial.print("Previous state: ");
                Serial.println(prevState);
            } else {
                Serial.println("Same state: ");
                Serial.println(state);
            }
        } else {
            Serial.println("Status: Could not connect");
        }
    }
}

unsigned char compareState(float a, float b) {
    if ((a > 0.0 && b > 0.0) || (a == 0.0 && b == 0.0)) {
        return 0;
    } else if (a > 0.0) {
        return -1;
    } else {
        return 1;
    }
}
