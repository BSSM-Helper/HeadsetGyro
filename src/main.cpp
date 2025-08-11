#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  if (!mpu.begin()) {
    Serial.println("MPU6050 찾을 수 없음!");
    while (1) delay(10);
  }
  Serial.println("MPU6050 연결 성공!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.printf("AccelX: %.2f, AccelY: %.2f, AccelZ: %.2f, ",
                a.acceleration.x, a.acceleration.y, a.acceleration.z);
  Serial.printf("GyroX: %.2f, GyroY: %.2f, GyroZ: %.2f\n",
                g.gyro.x, g.gyro.y, g.gyro.z);

  delay(100);
}
