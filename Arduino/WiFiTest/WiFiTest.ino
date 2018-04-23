#include <SoftwareSerial.h>
#define RX 8
#define TX 7

#define LEFT_F 9
#define LEFT_E 6
#define RIGHT_F 5
#define RIGHT_E 3

SoftwareSerial esp8266(RX, TX); 

String ipAddress = "192.168.43.1";
int rmtPort = 23333;
int rxPort = 11122;

int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  delay(5000);
  sendCommand("AT", 5, "OK");
  sendCmd("AT+CIFSR", 2000);
  sendCmd("AT+CWMODE=1",1000);
  sendCmd("AT+CWJAP=\"XZman\",\"1234567890123\"", 5000);
  sendCmd("AT+CIPMUX=0", 100);
  sendCmd(String("AT+CIPSTART=\"UDP\",") + "\"" + ipAddress + "\"," + rmtPort + "," + rxPort + ",0", 1000);
}

int LFDC = 0;
int LEDC = 0;
int RFDC = 0;
int REDC = 0;

void loop() {
  esp8266.listen();
  char leftSpeed = 0;
  char rightSpeed = 0;
  if (esp8266.find(':')) {
    leftSpeed = esp8266.read();
    rightSpeed = esp8266.read();
    Serial.print("leftSpeed: ");
    Serial.println((int)leftSpeed);
    Serial.print("rightSpeed: ");
    Serial.println((int)rightSpeed);

    LFDC = getLeftFrontDC((int)leftSpeed);
    LEDC = getLeftEndDC((int)leftSpeed);
    RFDC = getRightFrontDC((int)rightSpeed);
    REDC = getRightEndDC((int)rightSpeed);
  }
  Serial.println(String("LF:") + LFDC + "\tLE:" + LEDC + "\tRF:" + RFDC + "\tRE:" + REDC);
  analogWrite(LEFT_F, LFDC);
  analogWrite(LEFT_E, LEDC);
  analogWrite(RIGHT_F, RFDC);
  analogWrite(RIGHT_E, REDC);
}

int getLeftFrontDC(int ls) {
  return ls > 0 ? ls * 255 / 100: 0;
}

int getLeftEndDC(int ls) {
  return ls < 0 ? -ls * 255 / 100: 0;
}

int getRightFrontDC(int rs) {
  return rs > 0 ? rs * 255 / 100: 0;
}

int getRightEndDC(int rs) {
  return rs < 0 ? -rs * 255 / 100: 0;
}

// !!!!! assume len < 2048 !!!!!
void sendData(String s, int len, String ip, int port, int timeout) {
}

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
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
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
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
