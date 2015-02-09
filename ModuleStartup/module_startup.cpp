ModuleStartup::ModuleStartup(unsigned int port, char *message) {
    this.port = port;
    this.message = message;

    this.socket = UDP.begin(port);
}

IPAddress ModuleStartup::listenForRemoteIP() {
    Serial.println("Listening for an IP address");
    while (true) {
        IPAddress ip = this.run();
        if (ip) {
            return ip;
        }
        delay(500);
    }
}

IPAddress ModuleStartup::run() {
    char msg[10];
    if (this.socket.parsePacket() > 0) {
        this.socket.read(msg, 10);

        this.socket.flush();

        if (strcmp(msg, this.message) == 0) {
            Serial.print("Message: ");
            Serial.println(msg);
            Serial.print("Remote IP: ");
            Serial.println(this.socket.remoteIP());

            return this.socket.remoteIP();
        }
    }
    return NULL;
}
