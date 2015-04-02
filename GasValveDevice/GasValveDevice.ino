SYSTEM_MODE(SEMI_AUTOMATIC);
#define DEBUG_WIFI

#include "DeviceStartup.h"
#include "DeviceLifecycle.h"
#include "SparkIntervalTimer.h"

typedef enum {
    create_devices = 0,
    start_timer,
    check_status_when_ready
} StartupLifecycle;

IPAddress ccsIpAddress;
StartupLifecycle startupLifecycle;

DeviceLifecycle *dl;
float dlState;

volatile bool checkStatus = false;
IntervalTimer statusTimer;

void setup() {
    Serial.begin(9600);

    // semi-automatic mode doesn't connect automatically
    WiFi.on();
    WiFi.connect();

    while (!WiFi.ready()) SPARK_WLAN_Loop();

    startupLifecycle = create_devices;

    DeviceStartup *ds = new DeviceStartup(10000, "HAL CCS");
    ccsIpAddress = ds->listenForRemoteIP();
    Serial.println(ccsIpAddress);

    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);

    dl = new DeviceLifecycle(ccsIpAddress, "gas_valve");
}

void loop(void) {
    // Check status
    if (startupLifecycle == check_status_when_ready) {
        if (checkStatus) {
            performStatusCheck();
            checkStatus = false;
        }
    // Setup timer
    } else if (startupLifecycle == start_timer) {
        statusTimer.begin(setCheckStatus, 4000, hmSec);
        startupLifecycle = check_status_when_ready;
    // Create devices
    } else if (startupLifecycle == create_devices) {
        if (!dl->isValid) {
            dl->create("Gas Valve", "gas_valve");
        } else {
            startupLifecycle = start_timer;
        }
    }
}

void setCheckStatus(void) {
    Serial.println("Check status!");
    checkStatus = true;
}

void performStatusCheck(void) {
    if (WiFi.ready()) {
        if (dl->isValid && dl->getState(dlState)) {
            Serial.print("New state: ");
            Serial.println(dlState);
            if (dlState > 0.0) {
                Serial.println("Setting to ON/OPEN");
                digitalWrite(D5, LOW);
                digitalWrite(D6, HIGH);
            } else {
                Serial.println("Setting to OFF/CLOSED");
                digitalWrite(D5, HIGH);
                digitalWrite(D6, LOW);
            }
        }
    } else {
        Serial.println("Waiting for WiFi");
        delay(100);
    }
}
