import processing.serial.*;

Serial myPort;                // Create object from Serial class
String val;                  // Data received from the serial port

PVector loc, dir, acc;

void setup() 
{
  size(400, 400, P3D);
  loc = new PVector(width/2, height/2, 0);
  dir = new PVector(0, 0, 0);
  
  printArray(Serial.list());
  
  String portName = Serial.list()[2];
  myPort = new Serial(this, portName, 115200);
}

String toBytes(String input){
    String output = "";
    for(int i = 0; i < input.length(); i++){
      output += str(byte(input.charAt(i)));
    }
    return output;
}

void draw()
{
  if ( myPort.available() > 0) {              // If data is available,
    val = myPort.readStringUntil(10);         // read it and store it in val
    if(val == null){
      return;
    }
    if(val.contains("data:")){
      String[] data = split(val, ',');
      if(data.length < 11){
        println(val);
        return;
      }
      
      println(val);
      
      float w = float(data[4]) / 1000.0;
      float i = float(data[5]) / 1000.0;
      float j = float(data[6]) / 1000.0;
      float k = float(data[7]) / 1000.0;
      
      dir.x = atan2(2.0 * (j * k + i * w), (-sq(i) - sq(j) + sq(k) + sq(w)));
      dir.y = atan2(2.0 * (i * j + k * w), (sq(i) - sq(j) - sq(k) + sq(w)));
      dir.z = asin(-2.0 * (i * k - j * w) / (sq(i) + sq(j) + sq(k) + sq(w)));
      
      /*
      dir.x = float(data[5]) / 1000.0;
      dir.y = float(data[6]) / 1000.0;
      dir.z = float(data[7]) / 1000.0;
      */
          
      rectMode(CENTER);
      translate(loc.x, loc.y, loc.z);
      
      rotateX(-dir.x);
      rotateY(dir.y);
      rotateZ(dir.z);
      
      background(0);
      box(200);
      //rect(0, 0, 100, 100);
    }else{
      println(val); 
    }
  }
}
