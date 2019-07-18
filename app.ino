#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <SocketIoClient.h>

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;

unsigned long pulse_duration; //record the duration of low pulse
bool isFirstTimeConnect = true;

//PINS
int IR_in = 5;

void event(const char * payload, size_t length) {
  Serial.printf("got message: %s\n", payload);
}

void connect(const char * payload, size_t length) {
  Serial.printf("First time connecting...");
  String macAddress = WiFi.macAddress();
  String ipAddress = WiFi.localIP().toString;
  String i = "{\"mac\": \"" + macAddress + "\",\"ip\": \"" + ipAddress + "\"}";
  char identityPayload[100]; 
  webSocket.emit("identify", i.c_str());
}

const char* readPulseIn(){
  pulse_duration = pulseIn(IR_in, LOW, 10000000); //measure the duration of the low pulse
  String pulse_duration_string = (String) "\"" + pulse_duration + "\"";
  const char* pulse_duration_char;
  Serial.print(pulse_duration_char);
  return pulse_duration_string.c_str();
}


void setup() {
    Serial.begin(115200);

    Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

      for(uint8_t t = 4; t > 0; t--) {
          Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
          Serial.flush();
          delay(1000);
      }

    WiFiMulti.addAP("SKYbroadbandc4fd", "494102151");

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.on("event", event);
    webSocket.on("connect", connect);
    webSocket.begin("192.168.0.33", 4000);
}

void loop() {
    webSocket.loop();
    webSocket.emit("pulseIn", readPulseIn());
}

