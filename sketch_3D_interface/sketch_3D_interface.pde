import processing.serial.*;

Serial myPort;                // Create object from Serial class
String val;                  // Data received from the serial port

PVector loc, dir, acc;

void setup() 
{
  size(800, 800, P3D);
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
      
      float abs_ijk = sqrt(sq(i) + sq(j) + sq(k));
      i /= abs_ijk; j /= abs_ijk; k /= abs_ijk;
      
      background(0);
      directionalLight(255, 255, 255, 0.0, 0.6, -0.8);
      
      pushMatrix();
      rectMode(CENTER);
      translate(loc.x, loc.y);
      
      rotate(acos(w) * 2, -i, k, j);
      
      fill(0xff007fff); box(200);
      popMatrix();
    }else{
      println(val); 
    }
  }
}

public class Matrix{
   public float[][] vals;
                
   public Matrix(){
     vals = new float[4][4];
      for(int x = 0; x < 4; x++){
        for(int y = 0; y < 4; y++){
          vals[x][y] = 0;
        }
      }
   }
   
   public Matrix(float[][] _vals){
     vals = _vals;
   }
   
   public Matrix mult(Matrix other){
      Matrix ret = new Matrix(); 
      
      for(int i = 0; i < 4; i++){
         for(int j = 0; j < 4; j++){
           float val = 0;
           for(int k = 0; k < 4; k++){
             val += vals[i][k] * other.vals[k][j];
           }
           ret.vals[j][i] = val;
         }
      }
      
      return ret;
   }
}
