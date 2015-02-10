#include "HttpClient.h"
#include "JSMNSpark.h"
#include "DeviceLifecycle.h"

http_header_t headers[] = {
    { "Content-Type", "application/json" },
    { "Accept" , "application/json" },
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

HttpClient http;

DeviceLifecycle::DeviceLifecycle(IPAddress hostIp, char *deviceId)
 : hostIp(hostIp), deviceId(deviceId) {
    char *watchdogPath = new char[80];
    strcpy(watchdogPath, "/devices/");
    strcat(watchdogPath, this->deviceId);
    strcat(watchdogPath, "/watchdog/");

    this->watchdogPath = watchdogPath;
 }

bool DeviceLifecycle::performWatchdog() {
    http_request_t request;
    http_response_t response;

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
