#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";

WiFiClient espClient;
PubSubClient client(espClient);

#define I2C_ADDR 0x20

void setup() {
  Wire.begin(0, 2);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    while (!client.connect("RollWatcher")) delay(500);
  }
  Wire.beginTransmission(I2C_ADDR);
  Wire.requestFrom(I2C_ADDR, 1);
  if (Wire.available()) {
    uint8_t value = Wire.read();
    String payload = "{\"slots\":[";
    for (int i = 0; i < 5; i++) {
      payload += (value & (1 << i)) ? "1" : "0";
      if (i < 4) payload += ",";
    }
    payload += "]}";
    client.publish("toilet_paper/status", payload.c_str());
  }
  client.loop();
  ESP.deepSleep(12LL * 60 * 60 * 1000000); // 12 hours
}
