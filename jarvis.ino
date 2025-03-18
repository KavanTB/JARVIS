#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Galaxy A51 D2FB";
const char* password = "ajhq9289";
const char* serverAddress = "http://127.0.0.1:5000"; // Replace with your server's IP and endpoint

String preDefinedPrompt = "Generate a JSON format code for turning the LED on, then turn it off";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Send the prompt every 10 seconds
  delay(10000);
  sendPromptToServer(preDefinedPrompt);
}

void sendPromptToServer(String prompt) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverAddress);
    http.addHeader("Content-Type", "application/json");

    // Create JSON document for the prompt
    StaticJsonDocument<256> doc;
    doc["prompt"] = prompt;

    String jsonString;
    serializeJson(doc, jsonString);
    Serial.println(jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      if (httpResponseCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Payload:");
        Serial.println(payload);
        processJson(payload); // Process the JSON received from the server
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
    } else {
      Serial.print("Error sending request: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void processJson(String jsonString) {
  // Use ArduinoJson to parse the JSON string
  StaticJsonDocument<512> doc; // Adjust size as needed
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("sequence")) {
    JsonArray sequence = doc["sequence"].as<JsonArray>();
    for (JsonObject action : sequence) {
      String functionName = action["function"].as<String>();
      int delayTime = action["delay"].as<int>();

      if (functionName == "ledOn") {
        ledOn();
      } else if (functionName == "ledOff") {
        ledOff();
      } else {
        Serial.println("Unknown function: " + functionName);
      }
      delay(delayTime);
    }
  }
}

void ledOn() {
  Serial.println("Executing: ledOn");
  digitalWrite(LED_BUILTIN, HIGH); // Or whatever pin your LED is connected to
}

void ledOff() {
  Serial.println("Executing: ledOff");
  digitalWrite(LED_BUILTIN, LOW);
}