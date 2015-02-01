unsigned int halPort = 10901;
unsigned int nextTime = 0;    // Next time to contact the server
char udpMessage[10];

UDP Udp;

void setup() {
  Udp.begin(halPort);

  Serial.begin(9600);
  Serial.println(WiFi.localIP());
}

void loop() {
  if (nextTime > millis()) {
      return;
  }

  Serial.println("Start of loop");
  if (Udp.parsePacket() > 0) {
    // Read first char of data received
    Udp.read(udpMessage, 10);

    // Ignore other chars
    Udp.flush();

    // Store sender ip and port
    IPAddress ipAddress = Udp.remoteIP();
    int port = Udp.remotePort();
    Serial.print("Message: ");
    Serial.println(udpMessage);
    Serial.print("CCS IP: ");
    Serial.println(ipAddress);
  }

  nextTime = millis() + 10000;
}
