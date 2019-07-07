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
unsigned long prev_duration;
unsigned long pre_prev_duration;

int i = 0;

//pins
int IR_in = 5;

void event(const char * payload, size_t length) {
  Serial.printf("got message: %s\n", payload);
}

void readRate(){
  pulse_duration = pulseIn(IR_in, LOW, 10000000); //measure the duration of the low pulse
  //drip_rate_sec = 1000000 / pulse_duration; //get the frequency of the drip
  if (pulse_duration > 15000){ //capping at 60 drips/second
    drip_rate_sec = 1000000.00 / pulse_duration;
    drip_rate = drip_rate_sec * 60;
    drip_rate_sec_string = String(drip_rate_sec, 0);
    drip_rate_string = String(drip_rate, 0);
    Serial.println((String) digitalRead(IR_in) + ": " + pulse_duration + ", "
      + "drip rate is " + drip_rate_sec_string + "/sec | " + drip_rate_string + "/min");
  } 
   Serial.println((String) drip_rate);
   Serial.println((String) pulse_duration);
   Serial.println(digitalRead(IR_in));
  //prev_duration = pulse_duration;
  if (i == 0) {
      prev_duration = pulse_duration;
      i++;
      Serial.println("ONE");
  } if (i == 1) {
      if (prev_duration == 0 && pulse_duration == 0){
        pre_prev_duration = prev_duration;
        prev_duration = pulse_duration;
        i++;
        Serial.println("TWO");
      }
  } if (i == 2){
    if (prev_duration == 0 && pre_prev_duration == 0 && pulse_duration == 0){
      drip_rate = 0;
      i = 0;
      Serial.println("ZERO");
    }
  }
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
    webSocket.begin("192.168.0.33", 4000);
}

void loop() {
    webSocket.loop();
		delay(1000);
    readRate();
    String dps_string = "\"" + drip_rate_sec_string + "\"";
    String dpm_string = "\"" + drip_rate_string + "\"";
    char* dps;
    char* dpm;
    strcat(dps, dps_string.c_str());
    strcat(dpm, dpm_string.c_str());
    webSocket.emit("dps", dps);
    webSocket.emit("dpm", dpm);
}

