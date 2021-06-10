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
      
      float[][] temp = [
        [w, k, -j, i],
        [-k, w, i, j],
        [j, -i, w, k],
        [-i, -j, -k, w]
      ];
      Matrix a = new Matrix(temp);
      temp = [
        [w, k, -j, -i],
        [-k, w, i, -j],
        [j, -i, w, -k],
        [i, j, k, w]
      ];
      Matrix b = new Matrix(temp);
      
      rectMode(CENTER);
      translate(loc.x, loc.y, loc.z);
      
      resetMatrix();
      applyMatrix(a.mult(b));
      
      /*
      rotateX(-dir.x);
      rotateY(dir.y);
      rotateZ(dir.z);
      */
      
      background(0);
      box(200);
      //rect(0, 0, 100, 100);
    }else{
      println(val); 
    }
  }
}

public class Matrix{
   public float[][] vals;
                
   public Matrix(){
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
