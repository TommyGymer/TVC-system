# Arduino - ICM20948

This is the c++ written to be run by the Arduino.
It makes use of the Adafruit ICM20XX library to interact with the IMU.
It then integrates the data and then cross correlates the data to improve accuracy.
This data is then printed to the Serial as a quaternion.