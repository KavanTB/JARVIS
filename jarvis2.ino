#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "DFrobot_MSM261.h"

#define SAMPLE_RATE     (44100)
#define I2S_SCK_IO      (25)
#define I2S_WS_IO       (16)
#define I2S_DI_IO       (26)
#define DATA_BIT        (16)
#define MODE_PIN        (4)
DFRobot_Microphone microphone(I2S_SCK_IO, I2S_WS_IO, I2S_DI_IO);
char i2sReadrawBuff[100];

const char* ssid = "Galaxy A51 D2FB";
//const char* ssid = "TP-Link_B09A";
const char* password = "ajhq9289";
//const char* password = "89540824";
const char* serverAddress = "http://192.168.207.16:5000/generate"; // Replace with your server's IP and endpoint

String preDefinedPrompt = "The function ledOn() turns on the light. The function ledOff() turns off the light. Only output a JSON format. A sequence of functions should be in a list named 'action'. A function should be name 'function' and a delay should be named 'delay'. Can you make the light blink in morse code for 'SOS'?";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
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
  pinMode(MODE_PIN,OUTPUT);
  digitalWrite(MODE_PIN,LOW);//Configure the microphone as receiving data of left channel
  while(microphone.begin(SAMPLE_RATE, DATA_BIT) != 0){
      Serial.println(" I2S init failed");//Init failed
  }
  Serial.println("I2S init success");//Init succeeded
}

void loop() {
  // Send the prompt every 10 seconds
  //delay(5000);
  //sendPromptToServer(preDefinedPrompt);
  microphone.read(i2sReadrawBuff,100);
  //Output data of left channel
  Serial.println((int16_t)(i2sReadrawBuff[0]|i2sReadrawBuff[1]<<8));
  delay(100);
  
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

void ledOn() {
  Serial.println("Executing: ledOn");
  digitalWrite(LED_BUILTIN, HIGH); // Or whatever pin your LED is connected to
}

void ledOff() {
  Serial.println("Executing: ledOff");
  digitalWrite(LED_BUILTIN, LOW);
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
  // Now directly access "function" from the main object
  // if (doc.containsKey("function")) {
  //   String functionName = doc["function"].as<String>();  // Directly access from doc
  //   Serial.print("Function: ");
  //   Serial.println(functionName);
  //   if (functionName == "ledOn") {
  //     ledOn();
  // }
  if (doc.containsKey("action")) {
    JsonArray action = doc["action"].as<JsonArray>();
    for (JsonObject task : action) {
      String functionName = task["function"].as<String>();
      int delayTime = task["delay"].as<int>();
      delay(delayTime);
      if(functionName == "ledOn") {
        ledOn();
      }
      if(functionName == "ledOff") {
        ledOff();
      }
      
    }
  }
  // if (doc.containsKey("sequence")) {
  //   JsonArray sequence = doc["sequence"].as<JsonArray>();
  //   for (JsonObject action : sequence) {
  //     String functionName = action["function"].as<String>();
  //     int delayTime = action["delay"].as<int>();

  //     if (functionName == "ledOn") {
  //       ledOn();
  //     } else if (functionName == "ledOff") {
  //       ledOff();
  //     } else {
  //       Serial.println("Unknown function: " + functionName);
  //     }
  //     delay(delayTime);
  //   }
  // }
}


