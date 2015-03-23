SYSTEM_MODE(SEMI_AUTOMATIC);
#define DEBUG_WIFI

#include "DeviceStartup.h"
#include "DeviceLifecycle.h"
#include "SparkIntervalTimer.h"

typedef enum {
    create_devices = 0,
    start_timer_and_listening,
    check_status_when_ready
} StartupLifecycle;

typedef enum {
    listen_state_starting = 0,
    listen_state_check_available,
    listen_state_done
} ListenState;

IPAddress ccsIpAddress;
StartupLifecycle startupLifecycle;

DeviceLifecycle *dl1;
float dl1State;

DeviceLifecycle *dl2;
float dl2State;

volatile bool checkStatus = false;
IntervalTimer statusTimer;

TCPServer server(4567);
TCPClient client;

const char deviceIdLength = 100;
const char stateLength = 10;

void setup() {
    Serial.begin(9600);

    startupLifecycle = create_devices;

    // semi-automatic mode doesn't connect automatically
    WiFi.on();
    WiFi.connect();

    while (!WiFi.ready()) SPARK_WLAN_Loop();

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
        } else {
            checkForNewStatus();
        }
    // Setup timer
    } else if (startupLifecycle == start_timer_and_listening) {
        server.begin();

        //statusTimer.begin(setCheckStatus, 60000, hmSec);
        startupLifecycle = check_status_when_ready;
    // Create or update devices
    } else if (startupLifecycle == create_devices) {
        if (!dl1->isValid) {
            IPAddress localIp = WiFi.localIP();
            if (!dl1->create("Bottom Outlet", "outlet", localIp)) {
                delay(100);
                dl1->update("Bottom Outlet", "outlet", localIp);
            }
            delay(100);
        } else if (!dl2->isValid) {
            IPAddress localIp = WiFi.localIP();
            if (!dl2->create("Top Outlet", "outlet", localIp)) {
                delay(100);
                dl2->update("Top Outlet", "outlet", localIp);
            }
            delay(100);
        } else {
            startupLifecycle = start_timer_and_listening;
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

void checkForNewStatus(void) {
    if (WiFi.ready()) {
        //Serial.println("Listening for connections");
        listenForState();
        delay(100);
    } else {
        Serial.println("Waiting for WiFi");
        delay(100);
    }
}

ListenState server_state_machine = listen_state_starting;
unsigned long time_since;

void listenForState(void) {
    Serial.println("Listening for state");

    while (server_state_machine != listen_state_done) {
        if (server_state_machine == listen_state_check_available && client.connected()) {
            char deviceId[deviceIdLength + 1];
            parseMessage(deviceId, deviceIdLength);

            char stateValue[stateLength + 1];
            parseMessage(stateValue, stateLength);

            server_state_machine = listen_state_done;

            if (strlen(deviceId) > 1 && strlen(stateValue) > 1) {
                Serial.print("Device ID: ");
                Serial.println(deviceId);
                Serial.print("State: ");
                Serial.println(stateValue);

                float nextState = atof(stateValue);
                if (strcmp(dl1->getDeviceId(), deviceId) == 0) {
                    char res[20];
                    sprintf(res, "%f", nextState);
                    Serial.print("Next State: ");
                    Serial.println(res);
                    sprintf(res, "%f", dl1State);
                    Serial.print("Prev State: ");
                    Serial.println(res);
                    if (compareState(nextState, dl1State) != 0) {
                        dl1->setPrevState(dl1State);
                        dl1State = nextState;
                        changeDl1State(dl1State);
                    }
                } else if (strcmp(dl2->getDeviceId(), deviceId) == 0) {
                    if (compareState(nextState, dl2State) != 0) {
                        dl2->setPrevState(dl2State);
                        dl2State = nextState;
                        changeDl2State(dl2State);
                    }
                }
            }
        } else if (server_state_machine == listen_state_starting) {
            client = server.available();
            server_state_machine = listen_state_check_available;
            time_since = millis();
        }
    }

    server_state_machine = listen_state_check_available;
}

void parseMessage(char *message, char messageLength) {
    char index = 0;
    while (index < messageLength && client.available()) {
        Serial.println("Reading a byte");
        char currentIndex = index++;
        char currentByte = client.read();
        if (currentByte == '\n') {
            break;
        } else {
            message[currentIndex] = currentByte;
        }
    }
    message[index] = '\0';
}

void changeDl1State(float state) {
    if (state > 0.0) {
        Serial.println("Setting to HIGH");
        digitalWrite(D1, HIGH);
    } else {
        Serial.println("Setting to LOW");
        digitalWrite(D1, LOW);
    }
}

void changeDl2State(float state) {
    if (state > 0.0) {
        Serial.println("Setting to HIGH");
        digitalWrite(D2, HIGH);
    } else {
        Serial.println("Setting to LOW");
        digitalWrite(D2, LOW);
    }
}
