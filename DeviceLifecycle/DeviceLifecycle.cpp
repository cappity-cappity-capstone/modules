#include "HttpClient.h"
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

unsigned int ccsPort = 8080;

DeviceLifecycle::DeviceLifecycle(IPAddress hostIp, char *deviceId)
 : hostIp(hostIp), deviceId(deviceId), isValid(false) {
     createDevicePath();
     createStatePath();
}

void DeviceLifecycle::createDevicePath() {
    char *devicePath = new char[80];
    strcpy(devicePath, "/api/devices/");

    this->devicePath = devicePath;
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
    request.port = ccsPort;
    request.path = this->devicePath;

    char body[] = "{\"device_id\":\"%s\", \"name\":\"%s\", \"device_type\":\"%s\"}";
    char *requestBody = new char[80];
    sprintf(requestBody, body, this->deviceId, name, type);
    request.body = requestBody;

    http.post(request, response, headers);

    if (response.status == 201 || response.status == 409) {
        Serial.println("Done!");
        this->isValid = true;
    } else {
        Serial.print("Status: ");
        Serial.println(response.status);
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

    request.ip = this->hostIp;
    // Should be set to the local port we'll be listening on
    request.port = ccsPort;
    request.path = this->statePath;

    http.get(request, response, headers);

    float nextState;
    if (response.status == 200) {
        nextState = 0.0;
    } else if (response.status == 201) {
        nextState = 1.0;
    }

    if (compareState(nextState, state) != 0) {
        state = nextState;
        return true;
    } else {
        return false;
    }
}
