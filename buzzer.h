#include "arduino.h"
const byte buzzerPin = 16;
bool low_flag = false;
bool high_flag = true;

void configureBuzzerPin(){
  pinMode(buzzerPin, OUTPUT);
}

void low_buzzer(int a){
  if(low_flag==false){
    for (int i = a; i>0; i--){
        digitalWrite(buzzerPin,HIGH);
        delay(50);
        digitalWrite(buzzerPin,LOW);
        delay(80);
      }
      low_flag = true;
  }
}
void error_beef(){
  
    for (int i = 0; i<20; i++){
        digitalWrite(buzzerPin,HIGH);
        delay(100);
        digitalWrite(buzzerPin,LOW);
        delay(100);
      }
}
void success_beef(){
        digitalWrite(buzzerPin,HIGH);
        delay(500);
        digitalWrite(buzzerPin,LOW);
        delay(50);
}
void full_buzzer(int a){
    if(high_flag=false){
        for (int i =a;i>0;i--){
          digitalWrite(buzzerPin,HIGH);
          delay(100);
          digitalWrite(buzzerPin,LOW);
          delay(100);
           }
        high_flag=true;
      }
   }
