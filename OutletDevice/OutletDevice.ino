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

DeviceLifecycle *dl1;
float dl1State;

DeviceLifecycle *dl2;
float dl2State;

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

    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);

    dl1 = new DeviceLifecycle(ccsIpAddress, "outlet1");
    dl2 = new DeviceLifecycle(ccsIpAddress, "outlet2");
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
        if (!dl1->isValid) {
            dl1->create("Bottom Outlet", "outlet");
        } else if (!dl2->isValid) {
            dl2->create("Top Outlet", "outlet");
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
        if (dl1->isValid && dl1->getState(dl1State)) {
            Serial.print("New state: ");
            Serial.println(dl1State);
            if (dl1State > 0.0) {
                Serial.println("Setting to HIGH");
                digitalWrite(D1, HIGH);
            } else {
                Serial.println("Setting to LOW");
                digitalWrite(D1, LOW);
            }
        }
        Spark.process();
        if (dl2->isValid && dl2->getState(dl2State)) {
            Serial.print("New state: ");
            Serial.println(dl2State);
            if (dl2State > 0.0) {
                Serial.println("Setting to HIGH");
                digitalWrite(D2, HIGH);
            } else {
                Serial.println("Setting to LOW");
                digitalWrite(D2, LOW);
            }
        }
    } else {
        Serial.println("Waiting for WiFi");
        delay(100);
    }
}
