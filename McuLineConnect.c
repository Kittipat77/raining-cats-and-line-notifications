#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TridentTD_LineNotify.h>

// WiFi credentials
const char* ssid = "WifiID";
const char* password = "Wifipass";

// Line Notify token
const String lineToken = "S9jBh15n4LlaHoQCP5SIXgiN28BGqA8HDqZUxeWHP4Tz";

// Rain sensor pin
const int rainSensorPin = D0;
int rainSensorValue = 0;
// Buzzer pin
const int BUZZER_PIN = D1;  // Change this to the pin connected to the buzzer
const int BUZZER_FREQ = 2000; // Change this to adjust the frequency (in Hz) of the PWM signal

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize rain sensor
  pinMode(rainSensorPin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Check for rain

/*
  rainSensorValue = digitalRead(rainSensorPin);
  Serial.println(rainSensorValue);
  if (rainSensorValue == LOW) {
    Serial.println("It's raining!");
    soundBuzzer(10000);
    delay(3000);
  }*/

  // Example usage of Line_Notify function
  Line_Notify("Hello from ESP8266!");
  delay(10000); // Send message every 5 seconds

  // Other loop code...
}


void Line_Notify(String message) {
  Serial.println(LINE.getVersion());
  LINE.setToken(lineToken);
  LINE.notify(message);
}

void soundBuzzer(unsigned long duration) {
  analogWriteFreq(BUZZER_FREQ); // Set the PWM frequency
  analogWrite(BUZZER_PIN, 128); // Set PWM duty cycle to 100% (max volume)
  delay(duration); // Sound for 1 second
  analogWrite(BUZZER_PIN, 0); // Turn off the buzzer
  delay(1000); // Wait for 1 second before repeating
}
