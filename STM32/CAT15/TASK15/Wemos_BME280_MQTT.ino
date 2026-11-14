// ESP8266 Wemos D1 mini / D1 R2. Nucleo PA9 -> GPIO14, common GND.
// Public, unencrypted test broker: use only non-sensitive demo readings.
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <stdlib.h>

const char WIFI_SSID[] = "SNUC";
const char WIFI_PASSWORD[] = "snu12345";
const char MQTT_HOST[] = "test.mosquitto.org";
const uint16_t MQTT_PORT = 1883;

// Numeric GPIO avoids differences between board pin aliases.
SoftwareSerial sensorSerial(14, -1); // receive only; GPIO14 = D5 on D1 mini
WiFiClient network;
PubSubClient mqtt(network);
char clientId[40];
char topic[80];
char line[100];
size_t used = 0;
bool discardLine = false;
char payload[160];
bool pending = false;
uint32_t sampleTime = 0;
uint32_t lastAttempt = 0;
uint32_t lastWifiAttempt = 0;

void acceptLine(char *text) {
  // Parse exactly temperature_C,pressure_hPa,humidity_percent.
  // This rejects the greeting, missing values, extra fields and NaN/Inf.
  double values[3];
  char *cursor = text;
  for (int i = 0; i < 3; ++i) {
    char *end;
    values[i] = strtod(cursor, &end);
    if (end == cursor || !isfinite(values[i])) return;
    if (i < 2) {
      if (*end != ',') return;
      cursor = end + 1;
    } else if (*end != '\0') return;
  }
  if (values[0] < -40 || values[0] > 85 ||
      values[1] < 300 || values[1] > 1100 ||
      values[2] < 0 || values[2] > 100) return;

  int n = snprintf(payload, sizeof(payload),
    "{\"temperature_c\":%.2f,\"pressure_hpa\":%.2f,\"humidity_percent\":%.2f}",
    values[0], values[1], values[2]);
  if (n > 0 && n < (int)sizeof(payload)) {
    pending = true;
    sampleTime = millis();
    Serial.print("Sensor: ");
    Serial.println(payload);
  }
}

void readSensor() {
  if (sensorSerial.overflow()) {
    // Resynchronize at a newline after serial buffer overflow.
    used = 0;
    discardLine = true;
  }
  while (sensorSerial.available()) {
    char c = (char)sensorSerial.read();
    if (c == '\n') {
      if (!discardLine && used) {
        line[used] = '\0';
        acceptLine(line);
      }
      used = 0;
      discardLine = false;
    } else if (c != '\r' && !discardLine) {
      if (c < 32 || c > 126 || used >= sizeof(line) - 1) {
        discardLine = true;
        used = 0;
      } else line[used++] = c;
    }
  }
}

void setup() {
  Serial.begin(115200);
  sensorSerial.begin(9600);
  snprintf(clientId, sizeof(clientId), "nucleo-bme280-%06lx",
           (unsigned long)ESP.getChipId());
  snprintf(topic, sizeof(topic), "stm32-demo/%s/readings", clientId);
  Serial.println();
  Serial.print("Subscribe to: ");
  Serial.println(topic);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setSocketTimeout(2);
  lastAttempt = millis() - 5000UL;
}

void loop() {
  readSensor();
  uint32_t now = millis();
  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWifiAttempt >= 15000UL) {
      lastWifiAttempt = now;
      Serial.println("Waiting for Wi-Fi; retrying...");
      WiFi.reconnect();
    }
    delay(1);
    return;
  }
  if (!mqtt.connected() && now - lastAttempt >= 5000UL) {
    lastAttempt = now;
    Serial.println("Connecting to MQTT...");
    if (mqtt.connect(clientId)) Serial.println("MQTT connected");
    else {
      Serial.print("MQTT connection failed, state: ");
      Serial.println(mqtt.state());
    }
    // Connection calls can block; resynchronize instead of publishing partial data.
    while (sensorSerial.available()) sensorSerial.read();
    used = 0;
    discardLine = true;
    pending = false;
  }
  if (mqtt.connected()) {
    mqtt.loop();
    if (pending && millis() - sampleTime > 5000UL) pending = false;
    if (pending && mqtt.publish(topic, payload, false)) {
      pending = false;
      Serial.println("Published");
    }
  }
  delay(1);
}
