#include <SoftwareSerial.h>
#define RX 8
#define TX 7

#define LEFT_SPEED 9
#define LEFT_F 12
#define LEFT_E 13
#define RIGHT_SPEED 5
#define RIGHT_F 10
#define RIGHT_E 11

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
  
  delay(500);
  esp8266.begin(115200);
  delay(2000);
  sendCommand("AT", 5, "OK");
  sendCmd("AT+CIFSR", 2000);
  sendCmd("AT+CWMODE=1",1000);
  sendCmd("AT+CWJAP=\"XZman\",\"1234567890123\"", 5000);
  sendCmd("AT+CIPMUX=0", 100);
  sendCmd(String("AT+CIPSTART=\"UDP\",") + "\"" + ipAddress + "\"," + rmtPort + "," + rxPort + ",0", 1000);
}

void loop() {
  esp8266.listen();
  char lchar = 0;
  char rchar = 0;
  if (esp8266.find(':')) {
    lchar = esp8266.read();
    rchar = esp8266.read();
    int leftSpeed = (int) lchar;
    int rightSpeed = (int) rchar;
    Serial.print("leftSpeed: ");
    Serial.println(leftSpeed);
    Serial.print("rightSpeed: ");
    Serial.println(rightSpeed);

    bool forward = getDirection(leftSpeed, rightSpeed);
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
