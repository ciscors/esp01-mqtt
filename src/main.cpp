// This example uses an Adafruit Huzzah ESP8266
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://www.shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ESP8266WiFi.h>
#include <MQTT.h>
#include "Ticker.h"

#define WATER_CHANELL  "zgwAA54/alarm"

const char ssid[] = "dd-wrt";
const char pass[] = "19731998";

WiFiClient net;
MQTTClient client;
Ticker alarm;


unsigned long lastMillis = 0;

int ledPin = 2; 
#define in_led 0

void changeState()
{
  digitalWrite(in_led,!digitalRead(in_led));
}


void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "anatols", "test!2")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe(WATER_CHANELL);
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if(payload.startsWith("leek=0")) {
    Serial.print("Leek is stopped!\n");
    digitalWrite(in_led, LOW);
    alarm.detach();
  }

  if(payload.startsWith("leek=1")) {
    Serial.print("Leek is started!\n");
    alarm.attach(0.5,changeState);
  }
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

 
  client.begin("mx.acraft.in.ua", net);
  client.onMessage(messageReceived);
  pinMode(in_led, OUTPUT);
  digitalWrite(in_led, LOW);

  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 30000) {
    lastMillis = millis();
    client.publish("zgwAA54/Siren", "I'm here");
  }
}
