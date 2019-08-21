#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <HX711.h>

#include <SocketIoClient.h>

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;

unsigned long pulse_duration; //record the duration of low pulse
//unsigned long weight_value;
bool isFirstTimeConnect = true;

//PINS
int IR_in = 5;
//const int LOADCELL_DOUT_PIN = 14;
//const int LOADCELL_SCK_PIN = 12;

//HX711 scale;

void event(const char * payload, size_t length) {
  Serial.printf("got message: %s\n", payload);
}

void connect(const char * payload, size_t length) {
  Serial.println("First time connecting...");
  String macAddress = WiFi.macAddress();
  String ipAddress = WiFi.localIP().toString();
  String i = "{\"mac\": \"" + macAddress + "\",\"ip\": \"" + ipAddress + "\"}";
  const char* identity = i.c_str();
  Serial.println(identity);
  webSocket.emit("identify", identity);
}

const char* readValues(){
  pulse_duration = pulseIn(IR_in, LOW); //measure the duration of the low pulse
  String pulse_duration_string = (String) "\"" + pulse_duration + "\"";
  const char * pulse_duration_char = pulse_duration_string.c_str();
  //Serial.println(pulse_duration_char);
  webSocket.emit("pulseIn", pulse_duration_char);
  // weight_value = scale.get_units(10);
  // String weight_value_string = (String) "\"" + weight_value + "\"";
  // const char * weight_value_char = weight_value_string.c_str();
  // webSocket.emit("weighIn", weight_value_char);
}


void setup() {
    Serial.begin(115200);

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(-1063.64);


    Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

      for(uint8_t t = 4; t > 0; t--) {
          Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
          Serial.flush();
          delay(1000);
      }

    WiFi.mode(WIFI_STA);

    // WiFi.begin("auto-iv", "passwordispassword");
  
    // while (WiFi.status() != WL_CONNECTED) {
    //   delay(500);
    //   Serial.print(".");
    // }

    WiFiMulti.addAP("Flare S3 Power", "rageagainstthedyingofthelight");
    WiFiMulti.addAP("SKYbroadbandC4FD", "494102151");
    WiFiMulti.addAP("auto-iv", "passwordispassword");
    
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.on("event", event);
    webSocket.on("connect", connect);
    webSocket.begin("autoiv.xyz");
    connect("", 0);
}

void loop() {
    webSocket.loop();
    //webSocket.emit("pulseIn", readPulseIn());
    readValues();
    //webSocket.emit("hello", "\"hello\"");
}

