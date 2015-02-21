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
 : hostIp(hostIp), deviceId(deviceId) {
     createWatchdogPath();
     createStatePath();
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

    if (response.status == 200) {
        jsmn_parser p;
        jsmntok_t tok[10];

        jsmn_init(&p);

        char *body = new char[response.body.length() + 1];

        response.body.toCharArray(body, response.body.length() + 1);

        Serial.print("Body: ");
        Serial.println(body);

        int r = jsmn_parse(&p, body, tok, 10);

        if (r == JSMN_SUCCESS) {
            int numItems = tok[0].size;
            Serial.print("Num items: ");
            Serial.println(numItems);
            for (int i = 1; i < numItems + 1; i++) {
                if (tok[i].type == JSMN_STRING) {
                    char *token = new char[tok[i].size + 1];
                    strlcpy(token, body + tok[i].start, (tok[i].end - tok[i].start + 1));
                    Serial.print("String: ");
                    Serial.println(token);

                    switch(state_state_machine) {
                        case get_state_start:
                            if (strcmp(token, "state") == 0) {
                                state_state_machine = get_state_key;
                            } else {
                                state_state_machine = get_state_start;
                            }
                            break;
                        case get_state_key:
                            state = atof(token);
                            Serial.print("New State: ");
                            Serial.println(state);
                            state_state_machine = get_state_start;
                            break;
                        default:
                            state_state_machine = get_state_start;
                    }
                    delete[] token;
                }
            }
            delete[] body;
            return true;
        } else {
            delete[] body;
            Serial.println("Could not parse");
            return false;
        }
        return true;
    } else {
        return false;
    }
}
