
#include <Wire.h>
#include <Kalman.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#include "PID.h"

uint8_t i2c_data[14];
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;

uint32_t timer;

Kalman KalmanX;
Kalman KalmanY;
Kalman KalmanZ;

double KalAngleX;
double KalAngleY;
double KalAngleZ;

double gyroXangle;
double gyroYangle;

void setup() {

  Serial.begin(115200);
  Wire.begin();

#if ARDUINO >= 157
  Wire.setClock(400000UL); // Freq = 400kHz.
#else
  TWBR = ((F_CPU/400000UL) - 16) / 2; // Freq = 400kHz
#endif

  i2c_data[0] = 7;      // 0x19 - Sampling rate 8kHz/(7 + 1) = 1000Hz
  i2c_data[1] = 0x00;   // 0x1A - Disable FSYNC, Configure ACC 260Hz Filter, Configure 256Hz Gyro Filter, 8Khz Sampling
  i2c_data[2] = 0x00;   // 0x1B - Set Gyro full scale ± 250deg/s - Range
  i2c_data[3] = 0x00;   // 0x1C - Set Accelerometer Full Scale to ±2g - Range

  // i2c settings
  while(i2cWrite(0x19, i2c_data, 4, false));

  // PLL reference the X-axis gyro, disabling Sleep Mode
  while(i2cWrite(0x6B, 0x01, true));

  while(i2cRead(0x75, i2c_data, 1));

  if(i2c_data[0] != 0x68){
    Serial.print("Error. Board unknown\n");
    while(1){
      Serial.print("Error. Connect MPU6050 to i2c bus\n");
    }
  }

  // MPU6050 Sensor Stabilization Time
  delay(100);

  // 1 - Reading Acc XYZ Data
  while(i2cRead(0x3B, i2c_data, 14));

  // 2 - Organizing Acc XYZ Data
  accX = (int16_t)((i2c_data[0] << 8) | i2c_data[1]); // ([ MSB ] [ LSB ])
  accY = (int16_t)((i2c_data[2] << 8) | i2c_data[3]); // ([ MSB ] [ LSB ])
  accZ = (int16_t)((i2c_data[4] << 8) | i2c_data[5]); // ([ MSB ] [ LSB ])

  // 3 - Pitch and Roll Calculation
  double pitch = atan(accX/sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  double roll = atan(accY/sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;

  // 4 - Kalman XY Filter Initialization
  KalmanX.setAngle(roll);
  KalmanY.setAngle(pitch);

  gyroXangle = roll;
  gyroYangle = pitch;
 
  timer = micros();

  init_motores();
}

void loop() {

  // Reading the Acceleration and Gyro Data from the MPU6050 sensor
  while(i2cRead(0x3B, i2c_data, 14));

  // Acceleration
  accX = (int16_t)((i2c_data[0] << 8) | i2c_data[1]); // ([ MSB ] [ LSB ])
  accY = (int16_t)((i2c_data[2] << 8) | i2c_data[3]); // ([ MSB ] [ LSB ])
  accZ = (int16_t)((i2c_data[4] << 8) | i2c_data[5]); // ([ MSB ] [ LSB ])

  // Gyroscope
  gyroX = (int16_t)((i2c_data[8] << 8) | i2c_data[9]); // ([ MSB ] [ LSB ])
  gyroY = (int16_t)((i2c_data[10] << 8) | i2c_data[11]); // ([ MSB ] [ LSB ])
  gyroZ = (int16_t)((i2c_data[12] << 8) | i2c_data[13]); // ([ MSB ] [ LSB ])
  
  // Delta Time Calculation
  double dt = (double)(micros() - timer)/1000000;
  timer = micros();

  double pitch = atan(accX/sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  double roll = atan(accY/sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;

  // Converting from Rad/Second to Degrees/Second Gyroscope-based Angular Rate Calculation
  gyroXangle = gyroX / 131.0; //deg/s
  gyroYangle = gyroY / 131.0; //deg/s

  // Angle Estimation on X and Y Axes using Kalman Filter
  KalAngleX = KalmanX.getAngle(roll, gyroXangle, dt);
  KalAngleY = KalmanY.getAngle(pitch, gyroYangle, dt);

  double res = Compute(KalAngleY);

  PMWMotorControl(res);

  // Creating Dynamic Json to print on Serial
  DynamicJsonBuffer jBuffer;
  JsonObject& measure = jBuffer.createObject();
  JsonObject& data = jBuffer.createObject();

  measure["accx"] = accX;
  measure["accy"] = accY;
  measure["accz"] = accZ;
  measure["gyrox"] = gyroX;
  measure["gyroy"] = gyroY;
  measure["gyroz"] = gyroZ;
  measure["kalanglex"] = KalAngleX;
  measure["kalangley"] = KalAngleY;
  measure["pitch"] = pitch;
  measure["roll"] = roll;
  measure["res"] = res;
  measure["kp"] = kp;
  measure["kd"] = kd;
  measure["ki"] = ki;
  
  data["measure"] = measure;
  data.printTo(Serial);

  Serial.print("\n");

//  if (Serial.available() > 0){
//    char dados;
//    dados = Serial.read();
//    Serial.print(dados);
//    dados = dados.split(":");
//  }
}
