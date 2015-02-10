#include "DeviceStartup.h"

DeviceStartup::DeviceStartup(unsigned int port, char *message) {
    this->port = port;
    this->message = message;

    this->socket.begin(port);
}

IPAddress DeviceStartup::listenForRemoteIP() {
    Serial.println("Listening for an IP address");
    while (true) {
        Serial.println("Looking for IP");
        IPAddress ip = this->run();
        if (!(ip == INADDR_NONE)) {
            return ip;
        }
        delay(500);
    }
}

IPAddress DeviceStartup::run() {
    char msg[10];
    if (this->socket.parsePacket() > 0) {
        this->socket.read(msg, 10);

        this->socket.flush();

        if (strcmp(msg, this->message) == 0) {
            Serial.print("Message: ");
            Serial.println(msg);
            Serial.print("Remote IP: ");
            Serial.println(this->socket.remoteIP());

            return this->socket.remoteIP();
        }
    }
    return INADDR_NONE;
}