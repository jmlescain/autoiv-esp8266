#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <HX711.h>

#include <SocketIoClient.h>

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;

unsigned long pulse_duration; //record the duration of low pulse
unsigned long prev_pulse;
float weight_value;
bool isConnected = false;
int zero_count;

//PINS
int IR_in = 5;
const int LOADCELL_DOUT_PIN = 14;
const int LOADCELL_SCK_PIN = 12;

HX711 scale;

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
  isConnected = true;
  
}

void disconnect(const char * payload, size_t length){
  isConnected = false;
}

const char *readValues()
{
  pulse_duration = pulseIn(IR_in, LOW, 7000000); //measure the duration of the low pulse
  Serial.println(pulse_duration);
  if (pulse_duration == 0 && prev_pulse == 0) {
      zero_count++;
      //Serial.println("zero count: " + zero_count);
  } else {
    zero_count = 0;
  }
  if (pulse_duration >= 100000 || zero_count == 6) {
    if (zero_count == 6){
      //pulse_duration = 0;
      zero_count = 0;
    }
    weight_value = scale.get_units(10);            //measure weight
    String payload = (String) "{\"pulse\": \"" + pulse_duration + "\",\"weight\": \"" + weight_value + "\"}";
    const char *payload_char = payload.c_str();
    webSocket.emit("values", payload_char);
  }
  prev_pulse = pulse_duration;
}

void setup() {
    Serial.begin(115200);

    pinMode(IR_in, INPUT);

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(325.65); //324.87
    scale.set_offset(-121960); //211980

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

    WiFiMulti.addAP("Scopee Toffee", "toffeescopee");
    WiFiMulti.addAP("auto-iv", "passwordispassword");
    WiFiMulti.addAP("Flare S3 Power", "rageagainstthedyingofthelight");
    
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.on("event", event);
    webSocket.on("connect", connect);
    webSocket.on("disconnect", disconnect);
    webSocket.begin("autoiv.xyz");
}

void loop() {
    webSocket.loop();
    if (isConnected == true) {
      readValues();
    }
}

