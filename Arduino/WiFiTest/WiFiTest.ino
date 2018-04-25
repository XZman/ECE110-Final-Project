#include <SoftwareSerial.h>
#define RX 8
#define TX 7

#define LEFT_SPEED 11
#define LEFT_F 2
#define LEFT_E 4
#define RIGHT_SPEED 10
#define RIGHT_F 13
#define RIGHT_E 9

#define trigPin 12
#define echoPin 0
#define ledRed 3
#define ledGreen 5
#define ledBlue 6

SoftwareSerial esp8266(RX, TX); 

String ipAddress = "192.168.43.1";
int rmtPort = 23333;
int rxPort = 11122;

int countTimeCommand; 
bool found = false; 

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino online.");

  pinMode(LEFT_F, OUTPUT);
  pinMode(LEFT_E, OUTPUT);
  pinMode(RIGHT_F, OUTPUT);
  pinMode(RIGHT_E, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  
  delay(50);
  esp8266.begin(115200);
  delay(50);
  sendCommand("AT", 5, "OK");
  sendCmd("AT+CIFSR", 2000);
  sendCmd("AT+CWMODE=1",1000);
  sendCmd("AT+CWJAP=\"XZman\",\"1234567890123\"", 5000);
  sendCmd("AT+CIPMUX=0", 100);
  sendCmd(String("AT+CIPSTART=\"UDP\",") + "\"" + ipAddress + "\"," + rmtPort + "," + rxPort + ",0", 1000);
}

void loop() {
  /* wifi and motor control */
  esp8266.listen();
  char lchar = 0;
  char rchar = 0;
  bool forward = true;
  if (esp8266.find(':')) {
    lchar = esp8266.read();
    rchar = esp8266.read();
    int leftSpeed = (int) lchar;
    int rightSpeed = (int) rchar;
    Serial.print("leftSpeed: ");
    Serial.println(leftSpeed);
    Serial.print("rightSpeed: ");
    Serial.println(rightSpeed);

    forward = getDirection(leftSpeed, rightSpeed);
    Serial.println(forward);
    if (forward) {
      digitalWrite(LEFT_F, HIGH);
      digitalWrite(LEFT_E, LOW);
      digitalWrite(RIGHT_F, HIGH);
      digitalWrite(RIGHT_E, LOW);
    } else {
      digitalWrite(LEFT_F, LOW);
      digitalWrite(LEFT_E, HIGH);
      digitalWrite(RIGHT_F, LOW); 
      digitalWrite(RIGHT_E, HIGH);
    }

    analogWrite(LEFT_SPEED, abs(leftSpeed) * 128 / 100);
    analogWrite(RIGHT_SPEED, abs(rightSpeed)* 128 / 100);
  }

  /* ultrasonic and LED control */
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH,5000);
  distance = (duration * 1.0 / 2) / 29.1;
  int r = 0,
      g = 0,
      b = 0;
  bool stopMotor = false;
  if (20 <= distance && distance < 200) {
    r = -17 * distance / 12 + 850 / 3; 
    g = 255;
    b = 0;
  }   
  else if (5 <= distance && distance < 20) {
    r = 255; 
    g = 17 * distance - 85;
    b = 0;
  }
  else if (0 < distance && distance < 5) {
    r = 255; 
    g = 0;
    b = 0;
    stopMotor = true;
  }
  else if (distance >= 200 || distance <= 0) {
    Serial.println("Out of range");
    r = b = 0;
    g = 255;
  }
  analogWrite(ledRed, r);
  analogWrite(ledGreen, g);
  analogWrite(ledBlue, b);
  
  Serial.print(distance);
  Serial.println(" cm");
  Serial.println(String(r) + " " + g + " " + b);
  
  if (stopMotor && forward) {
    analogWrite(LEFT_SPEED, 0);
    analogWrite(RIGHT_SPEED, 0);
    Serial.println("Terrain. Engine stopped.");
  }
  
  delay(50);
}

// true for forward, false for backward
inline bool getDirection(int leftSpd, int rightSpd) {
  return leftSpd >= 0 && rightSpd >= 0;
}

inline int abs(int x) {
  return x >= 0 ? x : -x;
}

// !!!!! assume len < 2048 !!!!!
//void sendData(String s, int len, String ip, int port, int timeout) {
//}

String sendCmd(String cmd, int timeout) {
  String rx_str = "";
  esp8266.println(cmd);
  esp8266.listen();
  delay(timeout);
  while (esp8266.available() > 0) {
    char rx_byte = 0;
    rx_byte = esp8266.read();
    Serial.print(rx_byte);
    rx_str += rx_byte;
  }
  return rx_str;
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print("AT command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    
    if (readReplay == nullptr) {
      found = true;
      break;
    } else if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTimeCommand = 0;
  }
  
  found = false;
 }
