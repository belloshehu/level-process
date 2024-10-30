#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 4);


void pattern(){
    for(int i=0;i<16;i++){
      
              lcd.setCursor(i,0);
             lcd.write(B10100101);
             delay(50);
    }
      
   for(int x=1;x<3;x++){
              lcd.setCursor(15,x);
             lcd.write(B10100101);
             delay(50);
          }
   for(int i=15;i>=0;i--){
      
            lcd.setCursor(i,3);
             lcd.write(B10100101);
             delay(50);
    }
    for(int x=3;x>0;x--){
              lcd.setCursor(0,x);
             lcd.write(B10100101);
             delay(50);
          }  
   for(int x= 1;x<15;x++){
    lcd.setCursor(x,(x%2)+1);
    lcd.write(B10100101);
    delay(100);
    }
  }
void scrolling(){
  pattern();
 for(int num =0;num<1;num++){
    for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    delay(150);
  }
  for (int positionCounter = 0; positionCounter < 32; positionCounter++) {
    lcd.scrollDisplayRight();
    // wait a bit:
    delay(150);
  }
 
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(150);
  }
  delay(1000);
 }
}


void loop2(){
   for(int i =0;i<4;i++){ 
    if(i==0){
        for(int x =0; x<13;x++){
            if(x==0){
             lcd.setCursor(x,i);
             lcd.write(B10100101);
             delay(100);
            }
            else{
               //lcd.setCursor(x+1,i);
               lcd.write(B10100101);
               delay(100);
              }
   }
       for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    delay(150);
  }
  for (int positionCounter = 0; positionCounter < 32; positionCounter++) {
    lcd.scrollDisplayRight();
    // wait a bit:
    delay(150);
  }
 
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(150);
  }
    } 

  if(i==1){
        for(int x =1; x<14;x++){
             lcd.setCursor(x+1,i);
             lcd.write(B10100101);
             delay(100);
   }
    } 
  if(i==2){
        for(int x =0; x<15;x++){
             lcd.setCursor(x+1,i);
           lcd.write(B10100101);
             delay(100);
   }
  }
   if(i==3){
        for(int x =1; x<14;x++){
             lcd.setCursor(x+1,i);
             lcd.write(B10100101);
             delay(100);
    } 
    }   
  }
  }
