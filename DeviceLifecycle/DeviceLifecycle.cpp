#include "HttpClient.h"
#include "JSMNSpark.h"
#include "DeviceLifecycle.h"

http_header_t headers[] = {
    { "Content-Type", "application/json" },
    { "Accept" , "application/json" },
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

typedef enum {
    get_state_start = 0,
    get_state_key
} GetState;

HttpClient http;
static const http_request_t emptyRequest = {0};
static const http_response_t emptyResponse = {0};
http_request_t request;
http_response_t response;

DeviceLifecycle::DeviceLifecycle(IPAddress hostIp, char *deviceId)
 : hostIp(hostIp), deviceId(deviceId), isValid(false) {
     createDevicePath();
     createWatchdogPath();
     createStatePath();
}

void DeviceLifecycle::createDevicePath() {
    char *devicePath = new char[80];
    strcpy(devicePath, "/api/devices/");

    this->devicePath = devicePath;
}

void DeviceLifecycle::createWatchdogPath() {
    char *watchdogPath = new char[80];
    strcpy(watchdogPath, "/api/devices/");
    strcat(watchdogPath, this->deviceId);
    strcat(watchdogPath, "/watchdog/");

    this->watchdogPath = watchdogPath;
}

void DeviceLifecycle::createStatePath() {
    char *statePath = new char[80];
    strcpy(statePath, "/api/devices/");
    strcat(statePath, this->deviceId);
    strcat(statePath, "/state/");

    this->statePath = statePath;
}

void DeviceLifecycle::create(char *name, char *type) {
    request = emptyRequest;
    response = emptyResponse;

    request.ip = this->hostIp;
    request.port = 4567;
    request.path = this->devicePath;

    char body[] = "{\"device_id\":\"%s\", \"name\":\"%s\", \"device_type\":\"%s\"}";
    char *requestBody = new char[80];
    sprintf(requestBody, body, this->deviceId, name, type);
    request.body = requestBody;

    http.post(request, response, headers);

    if (response.status == 201 || response.status == 409) {
        this->isValid = true;
    }
}

bool DeviceLifecycle::performWatchdog() {
    request = emptyRequest;
    response = emptyResponse;

    request.ip = this->hostIp;
    request.port = 4567;
    request.path = this->watchdogPath;

    http.put(request, response, headers);

    if (response.status == 200) {
        return true;
    } else {
        return false;
    }
}

GetState state_state_machine = get_state_start;

unsigned char compareState(float a, float b) {
    if ((a > 0.0 && b > 0.0) || (a == 0.0 && b == 0.0)) {
        return 0;
    } else if (a > 0.0) {
        return -1;
    } else {
        return 1;
    }
}

bool DeviceLifecycle::getState(float &state) {
    request = emptyRequest;
    response = emptyResponse;

    Serial.println("Starting request for: ");
    Serial.print(this->hostIp);
    Serial.print(":");
    Serial.print(4567);
    Serial.println(this->statePath);

    request.ip = this->hostIp;
    // Should be set to the local port we'll be listening on
    request.port = 4567;
    request.path = this->statePath;

    http.get(request, response, headers);

    float nextState;
    if (response.status == 200) {
        nextState = 0.0;
    } else if (response.status == 201) {
        nextState = 1.0;
    }

    if (compareState(nextState, this->prevState) != 0) {
        this->prevState = state;
        state = nextState;
        return true;
    } else {
        return false;
    }
}

bool DeviceLifecycle::setState(float state) {
    request = emptyRequest;
    response = emptyResponse;

    Serial.println("Starting request for: ");
    Serial.print(this->hostIp);
    Serial.print(":");
    Serial.print(4567);
    Serial.println(this->statePath);

    request.ip = this->hostIp;
    // Should be set to the local port we'll be listening on
    request.port = 4567;
    request.path = this->statePath;

    // Setup the body
    char body[] = "{\"state\":\"%.2f\", \"source\":\"manual_override\"}";
    char *requestBody = new char[60];
    sprintf(requestBody, body, state);
    request.body = requestBody;

    http.post(request, response, headers);

    if (response.status == 200) {
        return true;
    } else {
        return false;
    }
}
