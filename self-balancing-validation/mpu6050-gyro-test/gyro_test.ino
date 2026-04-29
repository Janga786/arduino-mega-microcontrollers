#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// Variables to store the calibration offsets
float gyroX_offset = 0;
float gyroY_offset = 0;
float gyroZ_offset = 0;

void setup(void) {
  Serial.begin(9600);
  while (!Serial) delay(10);

  Serial.println("--- GYROSCOPE TEST ---");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050");
    while (1) delay(10);
  }
  
  // Set Gyro Range to 500 degrees/second (Good for hand-held testing)
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // --- CALIBRATION PHASE ---
  Serial.println("Hold the sensor COMPLETELY STILL for 3 seconds...");
  Serial.print("Calibrating");
  
  float xSum = 0, ySum = 0, zSum = 0;
  int numReadings = 500;

  for (int i = 0; i < numReadings; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    xSum += g.gyro.x;
    ySum += g.gyro.y;
    zSum += g.gyro.z;
    
    if (i % 50 == 0) Serial.print("."); // Print a dot every 50 readings
    delay(50);
  }
  
  // Calculate the average error
  gyroX_offset = xSum / numReadings;
  gyroY_offset = ySum / numReadings;
  gyroZ_offset = zSum / numReadings;

  Serial.println("\nDone!");
  Serial.print("Offsets -> X: "); Serial.print(gyroX_offset);
  Serial.print("  Y: "); Serial.print(gyroY_offset);
  Serial.print("  Z: "); Serial.println(gyroZ_offset);
  Serial.println("-------------------------------------------");
  Serial.println("Rot_X\tRot_Y\tRot_Z"); // Header for Serial Plotter
  delay(1000);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // 1. Read Raw Gyro Data
  float currentX = g.gyro.x;
  float currentY = g.gyro.y;
  float currentZ = g.gyro.z;

  // 2. Subtract the Offset (Calibration)
  // We also multiply by 57.29 to convert Radians/Sec to Degrees/Sec
  float rotX = (currentX - gyroX_offset) * 57.29578;
  float rotY = (currentY - gyroY_offset) * 57.29578;
  float rotZ = (currentZ - gyroZ_offset) * 57.29578;

  // 3. Print for Serial Plotter (Tab separated)
  Serial.print(rotX);
  Serial.print("\t");
  Serial.print(rotY);
  Serial.print("\t");
  Serial.println(rotZ);

  delay(50);
}