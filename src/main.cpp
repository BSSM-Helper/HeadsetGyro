#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define PUBLISH_INTERVAL 2000
#define EVENT_INTERVAL 200

// Wi-Fi 설정
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// MQTT 브로커 설정
const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic  = "sensors/mpu6050";

Adafruit_MPU6050 mpu;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("MPU6050Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  while (!mpu.begin()) {
    Serial.println("MPU6050 찾을 수 없음!");
    delay(10);
  }
  Serial.println("MPU6050 연결 성공!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  static sensors_event_t a, g, temp;
  static uint64_t eventCurrentMillis = millis();
  static uint64_t publishCurrentMillis = millis();

  if(eventCurrentMillis - millis() >= EVENT_INTERVAL) {
    mpu.getEvent(&a, &g, &temp);
    eventCurrentMillis = millis();
  }

  if(publishCurrentMillis - millis() >= PUBLISH_INTERVAL){
    char payload[256];
    snprintf(payload, sizeof(payload),
             "{\"accel\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f},"
             "\"gyro\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}}",
             a.acceleration.x, a.acceleration.y, a.acceleration.z,
             g.gyro.x, g.gyro.y, g.gyro.z);

    Serial.println(payload);

    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    client.publish(mqtt_topic, payload);

    publishCurrentMillis = millis();
  }
}
