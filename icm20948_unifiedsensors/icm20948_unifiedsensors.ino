// Demo for getting individual unified sensor data from the ICM20948
#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include "Quat.h"

Adafruit_ICM20948 icm;
Adafruit_Sensor *icm_temp, *icm_accel, *icm_gyro, *icm_mag;

#define ICM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define ICM_SCK 13
#define ICM_MISO 12
#define ICM_MOSI 11

Quat gyro_v;                                                        //gyro quaternion
Quat gyro_off;                                                      //gyro offset quaternion
Quat accl_off;                                                      //accelerometer offset quaternion
unsigned long currentTime, previousTime;                            //to calculate change in time
float elapsedTime;                                                  //calculated change in time
uint8_t gyro_divisor;                                               //divider to convert gyro data to useful information

//

void setup(void) {
  Serial.begin(115200);                                             //start the serial connection at 115200 baud
  while (!Serial)                                                   //wait for a serial connection
  Serial.println("Serial connected");
  delay(10);                                                        //pause to ensure connection
  
  if (!icm.begin_I2C()) {                                           //check if connected to the IC over I2C
    // if (!icm.begin_SPI(ICM_CS)) {                                //unused SPI connectors
    // if (!icm.begin_SPI(ICM_CS, ICM_SCK, ICM_MISO, ICM_MOSI)) {
    Serial.println("Failed to find ICM20948 chip");
    while (1) {
      delay(10);                                                    //if not connected, wait
      if(icm.begin_I2C()){
        break;
      }else{
        Serial.println("Failed to find ICM20948 chip");
      }
    }
  }

  Serial.println("Sensor found");

  //get sensors and print out sensor details
  icm_temp = icm.getTemperatureSensor();                            //get the temperature sensor
  icm_temp->printSensorDetails();                                   //print the sensor details

  icm_accel = icm.getAccelerometerSensor();                         //get the accelerometer
  icm_accel->printSensorDetails();                                  //print the sensor details

  icm_gyro = icm.getGyroSensor();                                   //get the gyroscope
  icm_gyro->printSensorDetails();                                   //print the sensor details

  icm_mag = icm.getMagnetometerSensor();                            //get the magnetometer
  icm_mag->printSensorDetails();                                    //print the sensor details

  gyro_divisor = icm.getGyroRateDivisor();                          //get the divisor for the gyro data

  //initialise the gyro quaternion to 0
  gyro_v.w = 1;

  //initialise values to calculate the first degree inherent offset
  float gyro_sum_x, gyro_sum_y, gyro_sum_z;
  gyro_sum_x = 0;
  gyro_sum_y = 0;
  gyro_sum_z = 0;

  float acc_sum_x, acc_sum_y, acc_sum_z;
  acc_sum_x = 0;
  acc_sum_y = 0;
  acc_sum_z = 0;

  //loop and sum measurements
  int repeats = 2000;
  int i = 1;
  while(i < repeats){
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sensors_event_t mag;
    icm_temp->getEvent(&temp);
    icm_accel->getEvent(&accel);
    icm_gyro->getEvent(&gyro);
    icm_mag->getEvent(&mag);
    
    gyro_sum_x += gyro.gyro.x / (gyro_divisor * 1.0);
    gyro_sum_y += gyro.gyro.y / (gyro_divisor * 1.0);
    gyro_sum_z += gyro.gyro.z / (gyro_divisor * 1.0);

    acc_sum_x += accel.acceleration.x;
    acc_sum_y += accel.acceleration.y;
    acc_sum_z += accel.acceleration.z - 9.81;

    i++;
  }

  //divide the summed measurements by the number of measurements to calculate an average offset from zero
  gyro_off.w = 0;
  gyro_off.i = gyro_sum_x / i;
  gyro_off.j = gyro_sum_y / i;
  gyro_off.k = gyro_sum_z / i;

  accl_off.w = 0;
  accl_off.i = acc_sum_x / i;
  accl_off.j = acc_sum_y / i;
  accl_off.k = acc_sum_z / i;
}

void loop() {
  //get the next measurement events from each of the sensors
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sensors_event_t mag;
  icm_temp->getEvent(&temp);
  icm_accel->getEvent(&accel);
  icm_gyro->getEvent(&gyro);
  icm_mag->getEvent(&mag);

  previousTime = currentTime;                               // Previous time is stored before the actual time read
  currentTime = micros();                                   // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000000.0;   // Divide by 1000000 to get seconds

  //serial plotter friendly format
  Serial.print("data:");
  Serial.print(temp.temperature);
  Serial.print(",");

  Quat acc_update(0, accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);
  acc_update = acc_update.sub(accl_off);

  Serial.print(acc_update.i);
  Serial.print(","); Serial.print(acc_update.j);
  Serial.print(","); Serial.print(acc_update.k);

  Quat val(0, gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
  val = val.div(gyro_divisor);
  val = val.sub(gyro_off);

  Quat gyro_update = val.axis_angle(elapsedTime);
  
  Quat est_grav = acc_update.rotate(gyro_v).normalised();
  Quat tilt_corr = est_grav.axis_angle_weight(0.8);

  gyro_v = tilt_corr.mult(gyro_v.mult(gyro_update));

   /**/
  Serial.print(","); Serial.print(gyro_v.w * 1000);
  Serial.print(","); Serial.print(gyro_v.i * 1000);
  Serial.print(","); Serial.print(gyro_v.j * 1000);
  Serial.print(","); Serial.print(gyro_v.k * 1000);
  /**/

  Serial.print(","); Serial.print(mag.magnetic.x);
  Serial.print(","); Serial.print(mag.magnetic.y);
  Serial.print(","); Serial.print(mag.magnetic.z);

  Serial.print(","); Serial.print(est_grav.w * 1000);
  Serial.print(","); Serial.print(est_grav.i * 1000);
  Serial.print(","); Serial.print(est_grav.j * 1000);
  Serial.print(","); Serial.print(est_grav.k * 1000);

  Serial.println();
}
