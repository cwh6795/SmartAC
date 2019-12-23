#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출
 
int blueTx=10;   //Tx (보내는핀 설정)at
int blueRx=11;   //Rx (받는핀 설정)
SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언
 
void setup() 
{
  Serial.begin(9600);   //시리얼모니터
  SBlueT.begin(9600); //블루투스 시리얼
}

void loop()
{
  if (SBlueT.available()) { // 블루투스 통신 확인
    Serial.write(SBlueT.read());  //블루투스측 내용을 시리얼모니터에 출력
  }
  if (Serial.available()) { // 시리얼 통신 입력 확인
    mySerial.write(Serial.read());  //시리얼 모니터 내용을 블루추스 측에 WRITE
  }
}

// AT
// AT+NAME:"[name]"
// AT+PSWD:"[pswd]"
