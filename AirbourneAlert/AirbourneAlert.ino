SYSTEM_MODE(SEMI_AUTOMATIC);
#define DEBUG_WIFI

#include "DeviceStartup.h"
#include "AlertLifecycle.h"

typedef enum {
    create_alerts = 0,
    check_status_when_ready
} StartupLifecycle;

IPAddress ccsIpAddress;
StartupLifecycle startupLifecycle;

AlertLifecycle *al;

void setup() {
    Serial.begin(9600);

    // semi-automatic mode doesn't connect automatically
    WiFi.on();
    WiFi.connect();

    while (!WiFi.ready()) SPARK_WLAN_Loop();

    startupLifecycle = create_alerts;

    DeviceStartup *ds = new DeviceStartup(10000, "HAL CCS");
    ccsIpAddress = ds->listenForRemoteIP();
    Serial.println(ccsIpAddress);

    pinMode(D1, INPUT_PULLUP);
    pinMode(D7, OUTPUT);

    al = new AlertLifecycle(ccsIpAddress, "airbourne_alert");
}

void loop(void) {
    // Check status
    if (startupLifecycle == check_status_when_ready) {
        performTriggerCheck();
    // Create devices
    } else if (startupLifecycle == create_alerts) {
        if (!al->isValid) {
            al->create("Gas Alert", "airbourne_alert");
        } else {
            startupLifecycle = check_status_when_ready;
        }
    }
}

int buttonTriggered;
int sensorReading;
float voltage;

float vpu = 3.3 / 4096.0;

void performTriggerCheck(void) {
    if (WiFi.ready()) {
        buttonTriggered = digitalRead(D1);
        // Triggered when low, not high
        if (buttonTriggered == LOW) {
            al->setTrigger(true);
        } else {
            sensorReading = analogRead(A6);
            voltage = vpu * (float)sensorReading;

            al->setTrigger((voltage > 1.9));
        }
    } else {
        Serial.println("Waiting for WiFi");
        delay(100);
    }
}
