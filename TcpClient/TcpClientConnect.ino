TCPClient client;

void setup() {
    Serial.begin(9600);
    delay(1000);

    while(!Serial.available()) SPARK_WLAN_Loop();

    Serial.println(WiFi.localIP());
    Serial.println(WiFi.subnetMask());
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.SSID());
}

void loop() {
    if (client.connect({10,0,0,7}, 4567)) {
        Serial.println("Connected");
    } else {
        Serial.println("Connection failed");
    }
    client.flush();
    client.stop();
    delay(1000);
}
