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

float gyro_vw, gyro_vx, gyro_vy, gyro_vz;                           //gyro quaternions
float gyro_off_x, gyro_off_y, gyro_off_z;                           //gyro offsets
unsigned long currentTime, previousTime;                            //to calculate change in time
float elapsedTime;                                                  //calculated change in time
uint8_t gyro_divisor;                                               //divider to convert gyro data to useful information

//

void setup(void) {
  Serial.begin(115200);                                             //start the serial connection at 115200 baud
  while (!Serial)                                                   //wait for a serial connection
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
  gyro_vw = 1;
  gyro_vx = 0;
  gyro_vy = 0;
  gyro_vz = 0;
  
  gyro_off_x = 0;
  gyro_off_y = 0;
  gyro_off_z = 0;

  //initialise values to calculate the first degree inherent offset
  float gyro_sum_x, gyro_sum_y, gyro_sum_z;
  gyro_sum_x = 0;
  gyro_sum_y = 0;
  gyro_sum_z = 0;

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

    i++;
  }

  //divide the summed measurements by the number of measurements to calculate an average offset from zero
  gyro_off_x = gyro_sum_x / i;
  gyro_off_y = gyro_sum_y / i;
  gyro_off_z = gyro_sum_z / i;
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

  Serial.print(accel.acceleration.x);
  Serial.print(","); Serial.print(accel.acceleration.y);
  Serial.print(","); Serial.print(accel.acceleration.z);
  Serial.print(",");

  float gyro_val_x = (gyro.gyro.x / gyro_divisor - gyro_off_x);
  float gyro_val_y = (gyro.gyro.y / gyro_divisor - gyro_off_y);
  float gyro_val_z = (gyro.gyro.z / gyro_divisor - gyro_off_z);
  float gyro_val_norm = sqrt(sq(gyro_val_x) + sq(gyro_val_y) + sq(gyro_val_z));
  float gyro_val_mag = gyro_val_norm * elapsedTime;
  float gyro_norm_x = gyro_val_x / gyro_val_norm;
  float gyro_norm_y = gyro_val_y / gyro_val_norm;
  float gyro_norm_z = gyro_val_z / gyro_val_norm;

  float quat_w = cos(gyro_val_mag / 2);
  float quat_i = gyro_norm_x * sin(gyro_val_mag / 2);
  float quat_j = gyro_norm_y * sin(gyro_val_mag / 2);
  float quat_k = gyro_norm_z * sin(gyro_val_mag / 2);

  //multiply the old quaternion by the new rotation quaternion
  float gyro_tw = gyro_vw * quat_w - gyro_vx * quat_i - gyro_vy * quat_j - gyro_vz * quat_k;
  float gyro_ti = gyro_vw * quat_i + gyro_vx * quat_w + gyro_vy * quat_k - gyro_vz * quat_j;
  float gyro_tj = gyro_vw * quat_j - gyro_vx * quat_k + gyro_vy * quat_w + gyro_vz * quat_i;
  float gyro_tk = gyro_vw * quat_k + gyro_vx * quat_j - gyro_vy * quat_i + gyro_vz * quat_w;
  
  gyro_vw = gyro_tw;
  gyro_vx = gyro_ti;
  gyro_vy = gyro_tj;
  gyro_vz = gyro_tk;

  /*
  //gyro.gyro.* are the gyro velocities
  gyro_vx = gyro_vx + (gyro_val_x);
  gyro_vy = gyro_vy + (gyro_val_y);
  gyro_vz = gyro_vz + (gyro_val_z);
  */

   /**/
  Serial.print(gyro_vw * 1000);
  Serial.print(","); Serial.print(gyro_vx * 1000);
  Serial.print(","); Serial.print(gyro_vy * 1000);
  Serial.print(","); Serial.print(gyro_vz * 1000);
  /**/

  Serial.print(",");
  Serial.print(mag.magnetic.x);
  Serial.print(","); Serial.print(mag.magnetic.y);
  Serial.print(","); Serial.print(mag.magnetic.z);

  Serial.println();
  delay(1);
}
