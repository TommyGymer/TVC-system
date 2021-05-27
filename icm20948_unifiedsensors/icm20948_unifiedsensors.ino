// Demo for getting individual unified sensor data from the ICM20948
#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>

Adafruit_ICM20948 icm;
Adafruit_Sensor *icm_temp, *icm_accel, *icm_gyro, *icm_mag;

#define ICM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define ICM_SCK 13
#define ICM_MISO 12
#define ICM_MOSI 11

float gyro_vx, gyro_vy, gyro_vz;
float gyro_dx, gyro_dy, gyro_dz;
float gyro_off_x, gyro_off_y, gyro_off_z;
unsigned long currentTime, previousTime;
float elapsedTime;

//

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  //Serial.println("Adafruit ICM20948 test!");

  if (!icm.begin_I2C()) {
    // if (!icm.begin_SPI(ICM_CS)) {
    // if (!icm.begin_SPI(ICM_CS, ICM_SCK, ICM_MISO, ICM_MOSI)) {
    Serial.println("Failed to find ICM20948 chip");
    while (1) {
      delay(10);
    }
  }

  //Serial.println("ICM20948 Found!");
  icm_temp = icm.getTemperatureSensor();
  icm_temp->printSensorDetails();

  icm_accel = icm.getAccelerometerSensor();
  icm_accel->printSensorDetails();

  icm_gyro = icm.getGyroSensor();
  icm_gyro->printSensorDetails();

  icm_mag = icm.getMagnetometerSensor();
  icm_mag->printSensorDetails();

  gyro_vx = 0;
  gyro_vy = 0;
  gyro_vz = 0;

  gyro_dx = 0;
  gyro_dy = 0;
  gyro_dz = 0;

  gyro_off_x = 0;
  gyro_off_y = 0;
  gyro_off_z = 0;

  long gyro_sum_x, gyro_sum_y, gyro_sum_z;
  gyro_sum_x = 0;
  gyro_sum_y = 0;
  gyro_sum_z = 0;

  //find offsets
  int repeats = 10000;
  int i = 1;
  while(i < repeats){
    sensors_event_t gyro;
    icm_gyro->getEvent(&gyro);
    
    gyro_sum_x += gyro.gyro.x;
    gyro_sum_y += gyro.gyro.y;
    gyro_sum_z += gyro.gyro.z;

    i++;
  }

  gyro_off_x = gyro_sum_x / i;
  gyro_off_y = gyro_sum_y / i;
  gyro_off_z = gyro_sum_z / i;
}

void loop() {
  //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sensors_event_t mag;
  icm_temp->getEvent(&temp);
  icm_accel->getEvent(&accel);
  icm_gyro->getEvent(&gyro);
  icm_mag->getEvent(&mag);

  previousTime = currentTime; // Previous time is stored before the actual time read
  currentTime = micros(); // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000000.0; // Divide by 1000 to get seconds

//  Serial.print("\t\tTemperature ");
//  Serial.print(temp.temperature);
//  Serial.println(" deg C");
//
//  /* Display the results (acceleration is measured in m/s^2) */
//  Serial.print("\t\tAccel X: ");
//  Serial.print(accel.acceleration.x);
//  Serial.print(" \tY: ");
//  Serial.print(accel.acceleration.y);
//  Serial.print(" \tZ: ");
//  Serial.print(accel.acceleration.z);
//  Serial.println(" m/s^2 ");
//
//  /* Display the results (rotation is measured in rad/s) */
//  Serial.print("\t\tGyro X: ");
//  Serial.print(gyro.gyro.x);
//  Serial.print(" \tY: ");
//  Serial.print(gyro.gyro.y);
//  Serial.print(" \tZ: ");
//  Serial.print(gyro.gyro.z);
//  Serial.println(" radians/s ");
//  Serial.println();
//
//  Serial.print("\t\tMag X: ");
//  Serial.print(mag.magnetic.x);
//  Serial.print(" \tY: ");
//  Serial.print(mag.magnetic.y);
//  Serial.print(" \tZ: ");
//  Serial.print(mag.magnetic.z);
//  Serial.println(" uT");
//  delay(100);

  //   serial plotter friendly format
  Serial.print("data:");
  Serial.print(temp.temperature);
  Serial.print(",");

  Serial.print(accel.acceleration.x);
  Serial.print(","); Serial.print(accel.acceleration.y);
  Serial.print(","); Serial.print(accel.acceleration.z);
  Serial.print(",");

  //this is the rotational acceleration rather than the rotational displacement
  gyro_vx = gyro_vx + ((gyro.gyro.x - gyro_off_x) * elapsedTime);
  gyro_vy = gyro_vy + ((gyro.gyro.y - gyro_off_y) * elapsedTime);
  gyro_vz = gyro_vz + ((gyro.gyro.z - gyro_off_z) * elapsedTime);

  //rotational velocity to rotational displacement
  gyro_dx = gyro_dx + (gyro_vx * elapsedTime);
  gyro_dy = gyro_dy + (gyro_vy * elapsedTime);
  gyro_dz = gyro_dz + (gyro_vz * elapsedTime);

  /**/
  Serial.print(gyro_dx);
  Serial.print(","); Serial.print(gyro_dy);
  Serial.print(","); Serial.print(gyro_dz);
  /**/
  /*
  Serial.print(gyro.gyro.x);
  Serial.print(","); Serial.print(gyro.gyro.y);
  Serial.print(","); Serial.print(gyro.gyro.z);
  */

  Serial.print(",");
  Serial.print(mag.magnetic.x);
  Serial.print(","); Serial.print(mag.magnetic.y);
  Serial.print(","); Serial.print(mag.magnetic.z);

  Serial.println();
  delay(100);
  
}
