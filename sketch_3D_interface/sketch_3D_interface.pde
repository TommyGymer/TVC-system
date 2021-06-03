import processing.serial.*;

Serial myPort;                // Create object from Serial class
String val;                  // Data received from the serial port

PVector loc, dir, acc;

void setup() 
{
  size(400, 400, P3D);
  loc = new PVector(width/2, height/2, 0);
  dir = new PVector(0, 0, 0);
  
  //printArray(Serial.list());
  
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
      
      println(val);
      
      dir.x = float(data[4]) / 1000.0;
      dir.y = float(data[5]) / 1000.0;
      dir.z = float(data[6]) / 1000.0;
          
      rectMode(CENTER);
      translate(loc.x, loc.y, loc.z);
      
      rotateX(dir.x * (30.0 / PI));
      rotateY(dir.y * (30.0 / PI));
      rotateZ(dir.z * (30.0 / PI));
      
      background(0);
      box(200);
      //rect(0, 0, 100, 100);
    }
  }
}
