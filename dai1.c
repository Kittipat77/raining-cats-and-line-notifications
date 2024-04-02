#include <ESP8266WiFi.h>

const char* ssid = "WifiID";
const char* password = "WifiPass";

void setup() {
    Serial.begin(115200);
    delay(10);

    // Connect to WiFi
    Serial.println();
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Optional: Check connection status
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected successfully");
    } else {
        Serial.println("Failed to connect to WiFi");
    }
}

void loop() {
    // loop
}

