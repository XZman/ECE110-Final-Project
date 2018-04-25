/*
 HC-SR04 Ping distance sensor]
 VCC to arduino 5v GND to arduino GND
 Echo to Arduino pin 13 Trig to Arduino pin 12
 Red POS to Arduino pin 11
 Green POS to Arduino pin 10
 560 ohm resistor to both LED NEG and GRD power rail
 More info at: http://goo.gl/kJ8Gl
 Original code improvements to the Ping sketch sourced from Trollmaker.com
 Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
 */

#define trigPin 12
#define echoPin 13
#define ledRed 3
#define ledGreen 5
#define ledBlue 6
#define speaker 10

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(speaker, OUTPUT);
}

void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  int r = 0,
      g = 0,
      b = 0,
      s = 0;
  if( 20 <= distance && distance < 200){
    r = -17 * distance / 12 + 850/3; 
    g = 255;
    b = 0;
  }   
  else if( 5<= distance && distance < 20){
    r = 255; 
    g = 17 * distance - 85;
    b = 0;
  }
  else if( 0< distance && distance < 5){
    r = 255; 
    g = 0;
    b = 0;
    s = - 10 * distance + 200;
  }
  else if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
    r = b = 0;
    g = 255;
  }
  analogWrite(ledRed, r);
  analogWrite(ledGreen, g);
  analogWrite(ledBlue, b);
  analogWrite(speaker, s);
  
  Serial.print(distance);
  Serial.println(" cm");
  Serial.println(String(r) + " " + g + " " + b);
  
  delay(50);
}
