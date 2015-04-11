#include "HttpClient.h"
#include "AlertLifecycle.h"

http_header_t headers[] = {
    { "Content-Type", "application/json" },
    { "Accept" , "application/json" },
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

HttpClient http;
static const http_request_t emptyRequest = {0};
static const http_response_t emptyResponse = {0};
http_request_t request;
http_response_t response;

unsigned int ccsPort = 8080;

AlertLifecycle::AlertLifecycle(IPAddress hostIp, char *alertId)
 : hostIp(hostIp), alertId(alertId), isValid(false), prevTrigger(false) {
     createAlertPath();
     createTriggerPath();
}

void AlertLifecycle::createAlertPath() {
    char *alertPath = new char[80];
    strcpy(alertPath, "/api/alerts/");

    this->alertPath = alertPath;
}

void AlertLifecycle::createTriggerPath() {
    char *triggerPath = new char[80];
    strcpy(triggerPath, "/api/alerts/");
    strcat(triggerPath, this->alertId);
    strcat(triggerPath, "/trigger/");

    this->triggerPath = triggerPath;
}

void AlertLifecycle::create(char *name, char *type) {
    request = emptyRequest;
    response = emptyResponse;

    request.ip = this->hostIp;
    request.port = ccsPort;
    request.path = this->alertPath;

    char body[] = "{\"alert_id\":\"%s\", \"name\":\"%s\", \"type\":\"%s\"}";
    char *requestBody = new char[80];
    sprintf(requestBody, body, this->alertId, name, type);
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

bool AlertLifecycle::setTrigger(bool trigger) {
    if (trigger == prevTrigger)
        return true;

    request = emptyRequest;
    response = emptyResponse;

    request.ip = this->hostIp;
    // Should be set to the local port we'll be listening on
    request.port = ccsPort;
    request.path = this->triggerPath;

    char body[] = "{\"state\":%s}";
    char *requestBody = new char[25];
    sprintf(requestBody, body, (trigger ? "true" : "false"));
    request.body = requestBody;

    http.post(request, response, headers);

    if (response.status == 201) {
        prevTrigger = trigger;
        return true;
    }
    return false;
}
