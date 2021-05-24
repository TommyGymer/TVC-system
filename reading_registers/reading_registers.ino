#include <Wire.h>;

byte addr = 0b1101000;

//register mappings from page 36 of the datasheet
//I2C sequences on page 30

void getRegisters(byte loc, unsigned char registers[], int len, bool mult){
  Wire.beginTransmission(addr);
  Wire.write(loc);
  Wire.endTransmission(false);
  
  Wire.requestFrom(addr, len);
  int i = 0;
  Serial.print("Bytes available: ");
  while(Wire.available() < len){}
  
  Serial.print(Wire.available());
  Serial.print(" of ");
  Serial.print(len);
  Serial.println(" requested.");
  
  while(Wire.available()){
    unsigned char l = 0;
    unsigned char h = 0;

    l = Wire.read();
    h = Wire.read();

    if(mult){
      Serial.println((h<<8)&0xFF00,HEX);
      Serial.println(l,HEX);
    }else{
      Serial.print(loc + i, HEX);     Serial.print(": "); Serial.print(l, HEX); Serial.print(": "); Serial.println(l, BIN);
      Serial.print(loc + i + 1, HEX); Serial.print(": "); Serial.print(h, HEX); Serial.print(": "); Serial.println(h, BIN);
    }
    
    registers[i] = h;
    registers[i+1] = l;
    i += 2;
  }
  Serial.print("Collected ");
  Serial.print(i);
  Serial.println(" bytes.");
}

void setRegister(byte loc, byte value){
  Wire.beginTransmission(addr);
  Wire.write(loc);
  Wire.write(value);
  Wire.endTransmission();
}

void setup() {
//  digitalWrite(17, LOW);
//  digitalWrite(18, LOW);
  
  Wire.begin();
  Serial.begin(115200);
  Serial.println();
  while (!Serial);
  Serial.println("Serial: connected");

  setRegister(0x7F, 0b00000000); //select user bank 0

  Wire.beginTransmission(addr);
  Wire.write(0x00);
  if(Wire.endTransmission(false)){
    Serial.println("error");
    //while(true){}
  }else{
    Serial.println("IMU: connected");
  }
  Wire.requestFrom(addr, 1);
  byte WHO_AM_I = Wire.read();
  Wire.endTransmission();
  Serial.println(WHO_AM_I, 16);
  if(WHO_AM_I == 0xEA){
    Serial.println("WHO_AM_I: correct");
  }else{
    Serial.println("WHO_AM_I: failed");
  }

  Wire.beginTransmission(addr);
  Wire.write(0x7F);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, 1);
  byte bank = Wire.read();
  Wire.endTransmission();
  Serial.print("Selected USER BANK: "); Serial.println(bank, BIN);

  //init
  //setRegister(0x06, 0b00000001); //exit sleep mode; set clock source
  
  setRegister(0x7F, 0b00000000); //select user bank 0
  
//  setRegister(0x06, 0b11000001); //reset all registers
//  delay(50);
  setRegister(0x06, 0b00000001); //exit sleep mode
  delay(50);
  setRegister(0x7F, 0b00100000); //select user bank 2

//  setRegister(0x05, 0b01110000/*0b01110000*/); //set all to cycled duty mode
//  
//  setRegister(0x03, 0b00000000); //first bit for DMP; second bit for FIFO (p52 for FIFO enable bits)
//  //setRegister(0x69, 0b00000000); //set FIFO to stream mode
//  setRegister(0x0F, 0b00000010); //set I2C to bypass mode
//
//  setRegister(0x7F, 0b00010000); //select user bank 1
//
//  setRegister(0x7F, 0b00100000); //select user bank 2
//  setRegister(0x02, 0b00111011); //enable gyro self-test
//  setRegister(0x15, 0b00111010); //enable acc self-test
//  delay(50);
//  setRegister(0x02, 0b00000000); //enable gyro self-test
//  setRegister(0x15, 0b00000000); //enable acc self-test
//
//  setRegister(0x7F, 0b00110000); //select user bank 3

  setRegister(0x7F, 0b00000000); //select user bank 0

  delay(30);

  int num_bytes = 12/*12*/;

  unsigned char registers[num_bytes];
  for(int i = 0; i < num_bytes; i++){
    registers[i] = 0;
  }
  getRegisters(0x00/*0x2D*/, registers, num_bytes, false);
  for(int i = 0; i < (num_bytes/2); i++){
    Serial.print(i + 1); Serial.print(": ");
    Serial.println(((registers[2*i] << 8) & 0xFF00) + (registers[2*i+1]), HEX);
  }
}

void loop() {
  int nDevices = 0;

  Serial.println("Scanning...");

  for (byte address = 1; address < 127; ++address) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("  !");

      ++nDevices;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
  delay(5000); // Wait 5 seconds for next scan
}
