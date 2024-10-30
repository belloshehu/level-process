/*   74HC595 PINS
DATA PIN-------->>>  14
CLOCK PIN ------>>>  11
LATCH PIN ------>>>  12
*/


const byte data_pin = 11; // pin 14 0n 74hc595
const byte clock_pin = 9; // pin 11 on 74hc595
const byte latch_pin = 10; // pin 12 on 74hc595                
byte blank =B00000000;
byte blk[6]={
       B00000000,  //empty tank
         B00000000,  //empty tank
           B00000000,  //empty tank
             B00000000,  //empty tank
               B00000000,  //empty tank
                 B00000000,  //empty tank
  };
byte full =B11111111;
byte led_pumping_pattern[6]={
                B00000000,  //empty tank
                B00000010,  // 20%
                B00000110,  // 40%
                B00001110,  // 60%
                B00011110,  // 80%
                B00111110  //  full tank
                 };
byte led_full_pattern[2]={
                B00000000,  //empty tank
                B00111110  //  full tank
                 };


            
void led_scroll(byte i){
       digitalWrite(latch_pin,LOW);
       shiftOut(data_pin,clock_pin,MSBFIRST,led_pumping_pattern[i]);
       digitalWrite(latch_pin, HIGH);
  }
void led_full(byte i){     
       digitalWrite(latch_pin,LOW);
       shiftOut(data_pin,clock_pin,MSBFIRST,led_full_pattern[i]);
       digitalWrite(latch_pin, HIGH);
  }
  void led_pumping(byte i){
 
      digitalWrite(latch_pin,LOW);
      shiftOut(data_pin,clock_pin,MSBFIRST,led_pumping_pattern[i]);
      digitalWrite(latch_pin, HIGH);
  }


void reset_register(){
            digitalWrite(latch_pin,LOW);
            shiftOut(data_pin,clock_pin,MSBFIRST,blank);
            digitalWrite(latch_pin, HIGH);
         
      }
      
void led_scroll2(){
     for(int i= 0; i<6; i++){
         digitalWrite(latch_pin,LOW);
         shiftOut(data_pin,clock_pin,MSBFIRST,led_pumping_pattern[i]);
         digitalWrite(latch_pin, HIGH);
         delay(50);
      }
  }

void led_not_pumping(){
     for(int i= 0; i<6; i++){
         digitalWrite(latch_pin,LOW);
         shiftOut(data_pin,clock_pin,MSBFIRST,blk[i]);
         digitalWrite(latch_pin, HIGH);
      }
  }
