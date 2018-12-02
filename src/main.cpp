#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0

#include "FastLED.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN 4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 100

const char* ssid = SSID_NAME;
const char* password = SSID_PASS;

const char* mqtt_host = MQTT_HOST;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASSWORD;

WiFiClient espClient;
PubSubClient client(mqtt_host, 1883, espClient);

float r = 0.0;
float g = 0.0;
float b = 0.0;

void callback(char* topic, byte* payload, unsigned int length) {
  if (length > 0) {
    switch(topic[strlen(topic) - 1]) {
    case 'r': r = atof((char*)payload); break;
    case 'g': g = atof((char*)payload); break;
    case 'b': b = atof((char*)payload); break;
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID

    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect

    Serial.print("Attempting to connect to: ");
    Serial.print(mqtt_host);
    Serial.println("");

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("drone/knob/r");
      client.subscribe("drone/knob/g");
      client.subscribe("drone/knob/b");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  client.setCallback(callback);

  FastLED.addLeds<LED_TYPE, DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  EVERY_N_MILLISECONDS( 20 ) {
    Serial.print("RGB: ");
    Serial.print(r);
    Serial.print(" ");
    Serial.print(g);
    Serial.print(" ");
    Serial.print(b);
    Serial.println("");

    fill_solid(leds, NUM_LEDS, CRGB((int)(r * 255), (int)(g * 255), (int)(b * 255)));
  }
  FastLED.show();
}
