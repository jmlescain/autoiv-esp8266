#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <SocketIoClient.h>

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;

//variables
float drip_rate_sec; //drip_rate_sec will hold the current drip rate per second
int drip_rate; //drip_rate will hold the current drip rate per minute, mostly what will be used in the app
String drip_rate_sec_string;
String drip_rate_string;

//variables for use in the program
unsigned long pulse_duration; //record the duration of low pulse
String pulse_duration_string;

int i = 0;

//pins
int IR_in = 5;

void event(const char * payload, size_t length) {
  Serial.printf("got message: %s\n", payload);
}

void firstConnect(const char * payload, size_t length) {
  Serial.printf("First time connecting...");
}

char* readRate(){
  pulse_duration = pulseIn(IR_in, LOW, 10000000); //measure the duration of the low pulse
  String pulse_duration_string = (String) "\"" + pulse_duration + "\"";
  char pulse_duration_char[12];
  pulse_duration_string.c_str();
  Serial.print(pulse_duration_char);
  return pulse_duration_char;
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
    webSocket.on("first-connect", firstConnect);
    webSocket.begin("192.168.0.33", 4000);
}

void loop() {
    webSocket.loop();
    webSocket.emit("pulseIn", readRate());
    webSocket.emit("test", "\"testing\"");
}

