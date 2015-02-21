TCPServer server = TCPServer(4567);
TCPClient client;

void setup() {
    server.begin();

    Serial.begin(9600);
    delay(1000);

    while(!Serial.available()) SPARK_WLAN_Loop();

    Serial.println(WiFi.localIP());
    Serial.println(WiFi.subnetMask());
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.SSID());
}

void loop() {
    if (client.connected()) {
        // echo all available bytes back to the client
        while (client.available()) {
            char output = client.read();
            Serial.print(output);
            server.write(output);
        }
    } else {
        // if no client is yet connected, check for a new connection
        client = server.available();
    }
}
