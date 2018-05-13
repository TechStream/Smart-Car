#include <SoftwareSerial.h>
/*
INSTRUCTIONS FOR SOFTWARE SERIAL:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)
*/
//software serial and pin initialization
SoftwareSerial bluetooth(10, 11); // RX, TX
const float pi = 3.14159267;
const float toDeg = 180/pi;
const float toRad = pi/180;
int maxValue = 255;
int leftPower = A0;
int rightPower = A1;
int leftForward = 4;
int leftBackward = 5;
int rightForward = 6;
int rightBackward = 7;

int red = A2;
int green = A3;
int blue = A4; 

//temporary variables 
String data;

void setup() {
  Serial.begin(9600);   
  while (!Serial); // wait for serial port to connect.
  bluetooth.begin(9600);
  pinMode(leftPower,OUTPUT);
  pinMode(rightPower,OUTPUT);
  pinMode(leftForward,OUTPUT);
  pinMode(leftBackward,OUTPUT);
  pinMode(rightForward,OUTPUT);
  pinMode(rightBackward,OUTPUT);
  //color pins
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(blue,OUTPUT);
  //initial color red
  analogWrite(red,255);
  Serial.println("Pins are initialized");
}
float degrees2radians(float degrees) {
  return degrees * toRad;
}
int convertTo(float val)
{
 return (val/10)*255;
}
int findQuad(int angle)
{
  if(angle>0 && angle<=90)
      return 1;
  if(angle>90 && angle<=180)
      return 2;
  if(angle>180 && angle<=270)
      return 3;
  if(angle>270 && angle<=360)
      return 4;
}
float abss(float n)
{
  if(n<0)
    return n*-1;
  return n;
}
void handlePins(int angle,int power)
{
  //base case
  if(power == 0)
  {
    analogWrite(leftPower,0);
    analogWrite(rightPower,0);
    digitalWrite(leftBackward,LOW);
    digitalWrite(leftForward,LOW);
    digitalWrite(rightBackward,LOW);
    digitalWrite(rightForward,LOW);
    return;
  }
  float x = power * abss(cos(degrees2radians(angle)));
  float y = power * abss(sin(degrees2radians(angle)));
  int quad = findQuad(angle);
  
  int x_m = maxValue - convertTo(x);
  int y_m = maxValue - convertTo(y);
  Serial.println("right="+String(x)+"; left="+String(y));
  Serial.print(quad);
  switch(quad)
  {
    case 1:
    digitalWrite(leftBackward,LOW);
    digitalWrite(leftForward,HIGH);
    digitalWrite(rightBackward,LOW);
    digitalWrite(rightForward,HIGH);
    analogWrite(leftPower,maxValue);
    analogWrite(rightPower,x_m);
    Serial.print("Left Power" + String( maxValue));
    Serial.println("Right Power" + String(x_m));
    break;
    case 2:
    digitalWrite(leftBackward,LOW);
    digitalWrite(leftForward,HIGH);
    digitalWrite(rightBackward,LOW);
    digitalWrite(rightForward,HIGH);
    analogWrite(leftPower,x_m);
    analogWrite(rightPower,maxValue);
    Serial.print("Left Power" + String(x_m));
    Serial.println("Right Power" + String(maxValue));
    break;
    case 3:
    digitalWrite(leftBackward,HIGH);
    digitalWrite(leftForward,LOW);
    digitalWrite(rightBackward,HIGH);
    digitalWrite(rightForward,LOW);
    analogWrite(leftPower,y_m);
    analogWrite(rightPower,maxValue);
    Serial.print("Left Power" + String(y_m));
    Serial.println("Right Power" + String(maxValue));
    
    break;
    case 4:
    digitalWrite(leftBackward,HIGH);
    digitalWrite(leftForward,LOW);
    digitalWrite(rightBackward,HIGH);
    digitalWrite(rightForward,LOW);
    analogWrite(leftPower,maxValue);
    analogWrite(rightPower,y_m);
    Serial.print("Left Power" + String(maxValue));
    Serial.println("Right Power" + String(y_m));
    break;
  } 
}
void processDATA(String d)
{
int index = d.indexOf(',');
int angle = (d.substring(0, index)).toInt();
int power = (d.substring(index+1)).toInt()/10;
handlePins(angle,power);
Serial.print(angle);
Serial.print("   ----   ");
Serial.println(power);
}
void handleColors()
{
  data = "";
  while (true)
  {
    while (bluetooth.available()) {
    data = data + (char)bluetooth.read();
    delay(10);   
    }
    if(data!=""){
      if(data.indexOf("color")!=-1)
      {
        Serial.println("RETURNING");
        return ;
      }
      else
      {
       Serial.println(data); 
       float color = data.toDouble();
        int r = (((int)color >> 16) & 0xff) ;
          int g = (((int)color >>  8) & 0xff);
        int b = (((int)color      ) & 0xff) ;  
        if(b>70 && g>70)
        r = 0;          
        Serial.println("Red "+ String(r)+" Green "+String(g)+" BLUE "+ String(b));
        analogWrite(red,r);
        analogWrite(green,g);
        analogWrite(blue,b);
      }
      data= "";
    }
    }
}
void loop() {
  while (bluetooth.available()) {
    data = data + (char)bluetooth.read();
    delay(10);   
  }
  
  if(data!=""){
    Serial.println("RECIE:  "+data);
    if(data.indexOf("col")!=-1)
    {
      Serial.println("GOING to handle");
      handleColors();
    }
    else
    {
       if(data.indexOf(",")!=-1){
          processDATA(data);
       }
    }
    data = "";
  }
}
