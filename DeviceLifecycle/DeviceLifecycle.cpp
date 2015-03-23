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
int ccsPort = 4567;

DeviceLifecycle::DeviceLifecycle(IPAddress hostIp, const char *deviceId)
 : hostIp(hostIp), deviceId(deviceId), isValid(false) {
     setCreateDevicePath();
     setUpdateDevicePath();
     setWatchdogPath();
     setStatePath();
}

void DeviceLifecycle::setCreateDevicePath() {
    char *devicePath = new char[80];
    strcpy(devicePath, "/api/devices/");

    this->createDevicePath = devicePath;
}

void DeviceLifecycle::setUpdateDevicePath() {
    char *devicePath = new char[80];
    strcpy(devicePath, "/api/devices/");
    strcat(devicePath, this->deviceId);

    this->updateDevicePath = devicePath;
}

void DeviceLifecycle::setWatchdogPath() {
    char *watchdogPath = new char[80];
    strcpy(watchdogPath, "/api/devices/");
    strcat(watchdogPath, this->deviceId);
    strcat(watchdogPath, "/watchdog/");

    this->watchdogPath = watchdogPath;
}

void DeviceLifecycle::setStatePath() {
    char *statePath = new char[80];
    strcpy(statePath, "/api/devices/");
    strcat(statePath, this->deviceId);
    strcat(statePath, "/state/");

    this->statePath = statePath;
}

void DeviceLifecycle::generateDeviceRequestBody(char *requestBody, char *name, char *type, char *ipAddress) {
    char body[] = "{\"device_id\":\"%s\", \"name\":\"%s\", \"device_type\":\"%s\", \"ip_address\":\"%s\"}";
    sprintf(requestBody, body, this->deviceId, name, type, ipAddress);
}

void DeviceLifecycle::generateIp(char *string, IPAddress ipAddress) {
    sprintf(string, "%d.%d.%d.%d", ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
}

bool DeviceLifecycle::create(char *name, char *type, IPAddress ipAddr) {
    request = emptyRequest;
    response = emptyResponse;

    request.ip = this->hostIp;
    request.port = ccsPort;
    request.path = this->createDevicePath;

    char *requestBody = new char[120];
    char *ipAddrString = new char[16];
    generateIp(ipAddrString, ipAddr);
    generateDeviceRequestBody(requestBody, name, type, ipAddrString);
    request.body = requestBody;

    http.post(request, response, headers);

    if (response.status == 201) {
        this->isValid = true;
        return true;
    }
    return false;
}

bool DeviceLifecycle::update(char *name, char *type, IPAddress ipAddr) {
    request = emptyRequest;
    response = emptyResponse;

    request.ip = this->hostIp;
    request.port = ccsPort;
    request.path = this->updateDevicePath;

    char *requestBody = new char[100];
    char *ipAddrString = new char[16];
    generateIp(ipAddrString, ipAddr);
    generateDeviceRequestBody(requestBody, name, type, ipAddrString);
    request.body = requestBody;

    http.put(request, response, headers);

    if (response.status == 200) {
        this->isValid = true;
        return true;
    }
    return false;
}

bool DeviceLifecycle::performWatchdog() {
    request = emptyRequest;
    response = emptyResponse;

    request.ip = this->hostIp;
    request.port = ccsPort;
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
    Serial.print(ccsPort);
    Serial.println(this->statePath);

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
    bool ret = (compareState(nextState, this->prevState) != 0);
    if (ret) {
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
    Serial.print(ccsPort);
    Serial.println(this->statePath);

    request.ip = this->hostIp;
    // Should be set to the local port we'll be listening on
    request.port = ccsPort;
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

void DeviceLifecycle::setPrevState(float state) {
    this->prevState = state;
}

float DeviceLifecycle::getPrevState(void) {
    return this->prevState;
}

const char *DeviceLifecycle::getDeviceId(void) {
    return this->deviceId;
}
