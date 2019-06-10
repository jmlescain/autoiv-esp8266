#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <SocketIoClient.h>

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;

void event(const char * payload, size_t length) {
  Serial.printf("got message: %s\n", payload);
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

    WiFiMulti.addAP("Flare S3 Power", "rageagainstthedyingofthelight");

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.on("event", event);
    webSocket.begin("192.168.43.203", 3000);
}

void loop() {
    webSocket.loop();
		delay(1000);
		webSocket.emit("greeting", "\"the esp8266 say hi!\"");
}