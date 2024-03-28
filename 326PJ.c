#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Mr.Freshh";
const char* password = "88888888n";

// Line Notify token
const String lineToken = "S9jBh15n4LlaHoQCP5SIXgiN28BGqA8HDqZUxeWHP4T";

// Rain sensor pin
const int rainSensorPin = D2;

// Buzzer pin
const int buzzerPin = D3;

// Weather API endpoint and API key
const String weatherAPIEndpoint = "https://api.openweathermap.org/data/2.5/forecast?id=1605466&appid=YOUR_API_KEY";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize rain sensor
  pinMode(rainSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Check for rain
  int rainSensorValue = digitalRead(rainSensorPin);
  if (rainSensorValue == HIGH) {
    Line_Notify("It's raining!");
    soundBuzzer(30000);
  }

  // Send daily weather forecast at 8:00 AM
  if (isTimeToSendWeatherForecast()) {
    String weatherForecast = sendWeatherForecast();
    Line_Notify(weatherForecast);
  }

  // Other loop code...
}

String parseWeatherData(String data) {
  // Create a DynamicJsonDocument object to parse the JSON data
  DynamicJsonDocument doc(1024);  // Adjust the capacity based on the size of the JSON data

  // Deserialize the JSON data
  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "Error parsing weather data";
  }

  // Extract the relevant weather information from the JSON data
  JsonObject forecast = doc["list"][0];  // Assuming we want the first forecast entry
  float temperature = forecast["main"]["temp"];
  float humidity = forecast["main"]["humidity"];
  String description = forecast["weather"][0]["description"];

  // Format the weather forecast string
  String weatherForecast = "Temperature: " + String(temperature) + " °C\n";
  weatherForecast += "Humidity: " + String(humidity) + "%\n";
  weatherForecast += "Description: " + description;

  return weatherForecast;
}

bool isTimeToSendWeatherForecast() {
  String thaiTime = getThailandTime();
  int hour = thaiTime.substring(11, 13).toInt();
  int minute = thaiTime.substring(14, 16).toInt();

  // Check if it's 8:00 AM
  if (hour == 8 && minute == 0) {
    return true;
  }

  return false;
}

String sendWeatherForecast() {
  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect("api.openweathermap.org", 443)) {
    client.print(String("GET ") + weatherAPIEndpoint + " HTTP/1.1\r\n" +
                 "Host: api.openweathermap.org\r\n" +
                 "Connection: close\r\n\r\n");

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        String weatherData = client.readStringUntil('\n');
        return parseWeatherData(weatherData);
      }
    }
  } else {
    Serial.println("Connection failed");
    return "Failed to fetch weather forecast";
  }
  
}

String getThailandTime() {
  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect("worldtimeapi.org", 80)) {
    client.print(String("GET /api/timezone/Asia/Bangkok HTTP/1.1\r\n") +
                 "Host: worldtimeapi.org\r\n" +
                 "Connection: close\r\n\r\n");

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        String response = client.readStringUntil('\n');
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, response);
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return "Error parsing time data";
        }
        return doc["datetime"].as<String>();
      }
    }
  } else {
    Serial.println("Connection failed");
    return "Failed to fetch time";
  }
}

void Line_Notify(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("Connection failed");
    return;
  }

  String payload = "\r\n\r\nmessage=" + message;
  String headers = "POST /api/notify HTTP/1.1\r\n";
  headers += "Host: notify-api.line.me\r\n";
  headers += "Authorization: Bearer " + lineToken + "\r\n";
  headers += "Content-Length: " + String(payload.length()) + "\r\n";
  headers += "\r\n";

  client.print(headers + payload);

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  client.stop();
  Serial.println("Line Notify message sent: " + message);
}

void soundBuzzer(unsigned long duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }
}