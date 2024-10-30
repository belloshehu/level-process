#include <Keypad.h>
#include <EEPROM.h>
#include "setups.h"
#include "boot.h"
#include "buzzer.h"
#include "led_display.h"
#include <NewPing.h>
#include <Wire.h> 
//#include <NewPingESP8266.h>
#include "tank.h"
#define TRIGGER_PIN  13  // trigger pin on the ultrasonic sensor to this Arduino pin.
#define ECHO_PIN     12  // echo pin on the ultrasonic sensor to this Arduino pin.
#define MAX_DISTANCE 200
NewPing sensor1(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// Set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x27, 16, 4);
long timeLimit = 10;
long lastTime  = 0; 
long last_sampling_time = 0;
int sample_sum = 0;
int level = 0;
byte sample_count = 0; 
byte row0 = 0;
byte row1 = 1;
byte row2 = 2;
byte row3 = 3;
// some set points
//bool full_flag = false;
int allowed_count =10000;
const int midSetpoint(80);
unsigned int gap{200};
unsigned long secs =0;
byte upper_setpoint_default =95;
byte lower_setpoint_default =70;
char * error, message;
//NewPingESP8266 sensor1(trig_pin,echo_pin);
//tank specification


struct setpoints{
    byte upper;
    byte lower;
    byte default_upper_setpoint =90;
    byte default_lower_setpoint =70;
  }my_setpoints;
struct messages{
    String error="Ok";
    String info= "Ok";
    bool state =false;
  
  }setpoints_messages;

struct machine{
    String message ="";
    String state ="";
  }pumping_machine;

struct user{
  const byte user_password_length =4;
  String password ="0000";
  }user1;
 struct manufacturer{
  String _name ="Sightek Nig. LTD";
  String _date ="01/02/2020";
  String _country ="Nigeria";
  String _phone ="09061983150";
  String _website ="www.sightek.cc";};
// flags declared here:
bool full_flag =false;
byte counting =0;

const byte relayPin =17;
const byte password_length=5;
char  password[password_length];
char oldPassword[password_length];
char newPassword[password_length];
String default_password ="1234";
//char default_password[password_length]={'1','2','3','4','5'};
char* welcome_message ="Water is precious, use it responsibly.";
char* notice ="You need password to set set points.";

void setup() {
    lcd.init();
    lcd.backlight();
    pinMode(latch_pin,OUTPUT);
    pinMode(clock_pin,OUTPUT);
    pinMode(data_pin,OUTPUT);
    pinMode(relayPin, OUTPUT);
    configureBuzzerPin();
    scrolling();
    //companyName();
    delay(1000);
    //loop2();
    reset_register();
    lcd.clear();
    //manufacturer_details();
    lcd.clear();
    //struct setpoints my_setpoints;
    if(EEPROM.read(all_addresses.upper_setpoint_address)==255 && EEPROM.read(all_addresses.lower_setpoint_address)==255){
        EEPROM.write(all_addresses.upper_setpoint_address, my_setpoints.default_upper_setpoint);
        EEPROM.write(all_addresses.lower_setpoint_address, my_setpoints.default_lower_setpoint);
        my_setpoints.upper = my_setpoints.default_upper_setpoint;
        my_setpoints.lower = my_setpoints.default_lower_setpoint;
        
      }
     else if(EEPROM.read(all_addresses.upper_setpoint_address)==255 && EEPROM.read(all_addresses.lower_setpoint_address)!=255){
        EEPROM.write(all_addresses.upper_setpoint_address, my_setpoints.default_upper_setpoint);
        my_setpoints.upper = my_setpoints.default_upper_setpoint;
        my_setpoints.lower = EEPROM.read(all_addresses.lower_setpoint_address);
      }
     else if(EEPROM.read(all_addresses.upper_setpoint_address)!=255 && EEPROM.read(all_addresses.lower_setpoint_address)==255){
        EEPROM.write(all_addresses.lower_setpoint_address, my_setpoints.default_lower_setpoint);
        my_setpoints.upper = EEPROM.read(all_addresses.upper_setpoint_address);
        my_setpoints.lower = my_setpoints.default_lower_setpoint;
       }
     else{
           my_setpoints.upper = EEPROM.read(all_addresses.upper_setpoint_address);
           my_setpoints.lower = EEPROM.read(all_addresses.lower_setpoint_address);    
        }       
    if(EEPROM.read(all_addresses.password_address[0])=='1' || EEPROM.read(all_addresses.password_address[0])==1 ){
         lcd.setCursor(0,0);
         lcd.print("  Personalise  ");
         lcd.setCursor(0,1);
         lcd.print(" your password");
         lcd.setCursor(16,0);
         lcd.print("   to secure");
         lcd.setCursor(16,1);
         lcd.print("  your device!");
         delay(2000);
       }
    else{
         //EEPROM.write(all_addresses.password_address, default_password);
         custom_eeprom_write(default_password);
      }
      //tank_height =200;
      //tank_allowed_maximum_level =190;
    bool isOk = false;
    while(isOk == false){
       if(!initialise_tank_parameters()){
          set_tank_parameters();
          initialise_tank_parameters();
          if( tank_height > 0){
                 lcd.clear();
                 lcd.setCursor(0,0);
                 lcd.print("Tank height:");
                 lcd.setCursor(4,1);
                 lcd.print(String(tank_height)+"cm.");
                 lcd.setCursor(16,0);
                 lcd.print(" Tank diameter");
                 lcd.setCursor(20,1);
                 lcd.print(String(tank_diameter)+"cm.");
                 delay(2000);
                 isOk =true;
            }
         
      }
      else{      lcd.clear();
                 lcd.setCursor(0,0);
                 lcd.print("Tank height:");
                 lcd.setCursor(4,1);
                 lcd.print(String(tank_height)+"cm.");
                 lcd.setCursor(16,0);
                 lcd.print(" Tank diameter");
                 lcd.setCursor(20,1);
                 lcd.print(String(tank_diameter)+"cm.");
                 delay(2000);
           isOk= true;
        }
    }
}

void wellcomeMessage(byte row, byte col,String message){
    for(byte i=col; i<message.length();i++){
        lcd.setCursor(i,row);
        lcd.print(message[i-col]);
        delay(100);
    }
}

void loop() {
  print_setpoints();
  if(millis() - last_sampling_time >= 5){
    sample_sum += get_level();
    sample_count +=1;
    last_sampling_time = millis();
    if(sample_count >= 20){
      level = sample_sum / sample_count;
      sample_count = 0;
      sample_sum = 0; 
      }
    }
  // take average of 20 level samples
//  for(byte i = 0; i < 20; i++){
//    sample_sum += get_level();
//    delay(10);
//  }
  print_level(level, pumping_machine.state);
  control_pumping_machine(level,my_setpoints.upper, my_setpoints.lower);
  char customKey =keypad.getKey();
  byte attempt_limit =3;
  byte attempt =0;
  if(customKey){
        if(customKey=='#'){//press # to set level set points
              lab:
              reset_display("Enter password:",0,0);
              struct user local_user = get_user_password(1,6);
              if(validate_password(local_user.password)){
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Access granted");
                          delay(2000);
                          label: struct setpoints both_setpoints =get_setpoints();
                          //update my_setpoints setpoints
                           my_setpoints.upper = both_setpoints.upper;
                           my_setpoints.lower = both_setpoints.lower;
                          bool state = set_setpoints(both_setpoints.upper, both_setpoints.lower);
                          lcd.clear();
                          lcd.setCursor(0,0);
                          delay(2000);
                          if (state){
                                lcd.print(setpoints_messages.info);
                                success_beef();
                                return;
                            }
                           else{
                                lcd.print(setpoints_messages.error);
                                error_beef();
                                goto label;
                            }
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("setpoints");
                          lcd.setCursor(0,1);
                          lcd.print(both_setpoints.upper);
                          lcd.setCursor(6,1);
                          lcd.print(both_setpoints.lower);
                          delay(2000);
                          }
                    else{  
                          attempt+=1;
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Wrong passowrd!");
                          lcd.setCursor(0,1);
                          lcd.print("Try again");
                          lcd.setCursor(17,1);
                          lcd.print("Attempt:"+String(attempt)+" of "+String(attempt_limit));
                          delay(2000);
                          if(attempt==attempt_limit){
                            return;
                            }
                            else{
                              goto lab;
                              }
                        }  
             }
         else if(customKey == 'C'){
              change_password();
              return;
          }
          else if(customKey =='D'){
              lcd.clear();
              manufacturer_details();
              return;             
            }
         else if(customKey == 'A'){
            lcd.clear();
            set_tank_parameters();
            return;
          }
         else if(customKey == '*'){
            lcd.clear();
            show_tank_parameters();
          }
      }
 }
 
void print_setpoints(){
     lcd.setCursor(16,1);
     lcd.print("Max:"+String(my_setpoints.upper)+String("%,"));
     lcd.setCursor(24,1);
     lcd.print("Min:"+String(my_setpoints.lower)+String("%"));
     delay(300);
     }
     
bool set_setpoints(byte upper_setpoint,byte lower_setpoint){
    byte upper_st = upper_setpoint;
    byte lower_st = lower_setpoint;
    if(upper_st == lower_st){
            setpoints_messages.error = "Max equals min!";
            setpoints_messages.state=false;
            return false;
      }
    else if(upper_st < lower_st){
            setpoints_messages.error = "Max less than min";
            setpoints_messages.state=false;
            return false;
      }
      else if(upper_st > lower_st && lower_st ==0){
            setpoints_messages.error = "Min. level is 0%";
            setpoints_messages.state=false;
            return false;
        }
    else if(upper_st ==0 && lower_st ==0){
            // save default setpoints
            EEPROM.update(all_addresses.upper_setpoint_address, upper_setpoint_default);
            EEPROM.update(all_addresses.lower_setpoint_address, lower_setpoint_default);
            setpoints_messages.info = "Defaults saved!";
            setpoints_messages.state=true;
            return true;
      }
    else{
            //save_setpoints(upper_st,lower_st);
            EEPROM.write(all_addresses.upper_setpoint_address, upper_st);
            EEPROM.write(all_addresses.lower_setpoint_address, lower_st);
            //custom_eeprom_setpoints_update(upper_st);
            //custom_eeprom_setpoints_update(lower_st);
            setpoints_messages.info = "Setpoints saved";
            setpoints_messages.state=true;
            return true;
        }
       
}

bool update_password(char* old_password,char* new_password ){
  if(EEPROM.read(all_addresses.password_address)){
      return true;
    }
  else{
      return false;
      }
  }
bool save_password(char new_password){
   EEPROM.update(all_addresses.password_address, new_password);
   if(EEPROM.read(all_addresses.password_address)){
      return true;
    }
   else{
      return false;
      }
   }

bool validate_password(String password){
  byte len =4;
  if(custom_eeprom_read(len).equals(password)){
      return true;
      }
   else{
      return false;
      }
  }
  
float get_level(){
    // returns water level in percentage
     int sum = 0;
     for(int i = 0; i<20; i++){
          int d_cm = sensor1.ping_cm();
          sum = sum+d_cm;
          delay(8);
      }
      byte average = sum/20;
      byte percent = to_percentage(average); 
      return percent;    
  }
void control_pumping_machine(byte level, byte upperSetpoint, byte lowerSetpoint){
     static byte led_counter =0;
     unsigned long last =0;
     if(full_flag == false){
              if(level < upperSetpoint && level >= lowerSetpoint){//if volume is less than uppersetpoint and greater than or equal to lower setpoint ,  
                    digitalWrite(relayPin,HIGH);
                    pumping_machine.state = "PUMPING";
                    low_buzzer(3);
                    full_flag = false;
                    if(millis() - last >= 100){
                         if(led_counter>1){
                           led_counter = 0;
                          }
                          led_full(led_counter);
                          led_counter++;
                          last = millis();
                     }
                      
                }
              else{//when volume is more than upper setpoint  
                    digitalWrite(relayPin,LOW);
                    pumping_machine.state = "NOT PUMPING";
                    full_buzzer(5);
                    full_flag = true;
                    led_not_pumping();
                     
                }
           }
        else{  //WHEN full_flag IS TRUE
               if(level<=midSetpoint ){//if volume is less than uppersetpoint,  
                   digitalWrite(relayPin,HIGH);
                   pumping_machine.state = "PUMPING";
                   //low_buzzer(3);
                    if(millis() - last >=100){
                         if(led_counter>1){
                           led_counter = 0;
                          }
                          led_full(led_counter);
                          led_counter++;
                          last = millis();
                     }
                }
               if(level >= upperSetpoint){//when volume is more than upper setpoint  
                    digitalWrite(relayPin,LOW);
                    pumping_machine.state = "NOT PUMPING";
                    full_buzzer(5);
                    led_not_pumping();
                }
           }
}

void print_level(int level,String machine_state){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Level(%): "+String(level));
      lcd.setCursor(0, 1); 
      // lcd.print(String(get_distance())+","+String(tank_diameter)+","+String(volume(get_distance(),tank_diameter)));
      lcd.print("Qty:"+String(volume(get_distance(),tank_diameter)/1000)+" Litres");
      lcd.setCursor(16, 0);
      lcd.print(" "+machine_state);
   }
   
char getPassword(){
  char password_mode;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter password->");
  byte entered_keyCount=0;
  while(entered_keyCount<password_length){
      char customKey = keypad.getKey();
      if(customKey){
            if(customKey=='D'){
                 //clear second row of screen when Delete button is pressed
                 lcd.clear();
                 lcd.setCursor(0,0);
                 lcd.print("Enter password:");
                 entered_keyCount = 0;
                 //continue;
                 }
            else if(customKey == 'C'){
                 lcd.clear();
                 lcd.setCursor(0,0);
                 lcd.print("Enter old p-word"); 
                 entered_keyCount = 0;
                 while(entered_keyCount<password_length){
                    char customKey =keypad.getKey();
                    if(customKey){
                         if(customKey == 'D'){
                                lcd.clear();
                                lcd.setCursor(0,0);
                                lcd.print("Enter old p-word");
                                entered_keyCount = 0;
                          }
                         else{   
                                oldPassword[entered_keyCount] =customKey;
                                lcd.setCursor(entered_keyCount,1);
                                lcd.print("*");
                                entered_keyCount +=1;  
                           }
                       }
                   }
                   lcd.clear();
                   lcd.setCursor(0,0);
                   lcd.print("Enter new p-word"); 
                   entered_keyCount = 0;
                   while(entered_keyCount<password_length){
                      char customKey =keypad.getKey();
                      if(customKey){
                          newPassword[entered_keyCount] =customKey;
                          lcd.setCursor(entered_keyCount,1);
                          lcd.print("*");
                          entered_keyCount+=1;
                        }
                    }
                     password_mode = 'c';
              }
          else{
                 password[entered_keyCount] =customKey;
                 lcd.setCursor(entered_keyCount,1);
                 lcd.print("*");
                 entered_keyCount+=1; 
                 password_mode ='n';
                }
              } 
          }
          entered_keyCount=0;
          lcd.setCursor(0,2);
          lcd.print("Password entered");
          lcd.setCursor(0,3);
          lcd.print("Please wait!");
          delay(4000); 
 }

 bool validateKeyForInteger(char key){
      if( key=='0' || key=='1' || key=='2' || key=='3' || key=='4' || key=='5' || key=='6' || key=='7' || key=='8' || key=='9'){
          return true;
          }
      else{
          return false;
          }
  }

unsigned long background_counter(){
  if(millis()-lastTime>=1000){
    secs+=1;
    lastTime=millis();
    return  secs;
    }
}

unsigned long counter(){
   static byte seconds;
  if(millis()-lastTime>=1000){
     lastTime=millis();
   }
   return 1;
}

 int  intArrayToValue(byte intArray[]){
    int val=0;
    for(int key =1; key >=0; key--){
            int decimal_value =pow(10,key);
            byte arrayElement = intArray[1-key];
            val = val+arrayElement*decimal_value;     
      }
     return val;
}

struct setpoints get_setpoints(){
   byte upper_setpoint[3],lower_setpoint[3] ={0,0,0};
   lcd.clear();
   lcd.setCursor(0,row0);
   lcd.print("Enter 80-99%");
   lcd.setCursor(0,row1);
   lcd.print("for max. level");
   lcd.setCursor(16,row0);
   lcd.print("Enter 60-75%");
   lcd.setCursor(16,row1);
   lcd.print("for min. level");
   byte entered_keyCount=0;
   bool done =false;
   static struct setpoints upper_lower_setpoints;
   while(done==false){
       char customKey =keypad.getKey();
       if(customKey){
              //count =0; // reset counting
              if(customKey =='A'){// press 'A' to set upper-setpoint
                      //reset_display("Enter upper sp.:",0,0);
                      lcd.clear();
                       lcd.setCursor(0,0);
                       lcd.print("Enter maximum");
                       lcd.setCursor(0,1);
                       lcd.print("level:");
                      byte display_row = 0;
                      upper_lower_setpoints.upper = set_upper_or_lower_setpoints(upper_setpoint,display_row,'u');
                }
              else if(customKey=='B'){// press 'B' to set lower-setpoint
                      //reset_display("Enter lower sp.:",0,0);
                       lcd.clear();
                       lcd.setCursor(0,0);
                       lcd.print("Enter minimum");
                       lcd.setCursor(0,1);
                       lcd.print("level:");
                      byte display_row = 0;
                      upper_lower_setpoints.lower  = set_upper_or_lower_setpoints(lower_setpoint, display_row,'l');
                   }
              else if(customKey =='*'){ //quite the loop otherwise
                        done=true;    
                  }    
       }
        
 }
 return upper_lower_setpoints;
}

int set_upper_or_lower_setpoints(byte setpoint_type[], byte display_row,char indicator){
        byte entered_key_count=0;
        bool setpoint_done =false;
        while(entered_key_count <2 || setpoint_done == false){
                    char entered_key =keypad.getKey();
                    if(entered_key){
                   
                           if(isDigit(entered_key)){
                                   setpoint_type[entered_key_count] =charToInt(entered_key);
                                   lcd.setCursor(23+entered_key_count,display_row);
                                   lcd.print(entered_key);
                                   entered_key_count+=1;
                                }
                            else if(entered_key == '*'){
                                   lcd.clear();
                                   lcd.setCursor(0,0);
                                   if(indicator=='u'){
                                        lcd.print("Upper setpoint");
                                        EEPROM.update(all_addresses.upper_setpoint_address,intArrayToValue(setpoint_type)); 
                                   }  
                                    else{
                                        lcd.print("Maximum level"); 
                                         EEPROM.update(all_addresses.lower_setpoint_address,intArrayToValue(setpoint_type)); 
                                    }
                                   lcd.setCursor(0,1);
                                   lcd.print("of "+String(intArrayToValue(setpoint_type))+"%");
                                   lcd.setCursor(16,0);
                                   lcd.print("Saved..........");
                                   delay(2000);
                                   setpoint_done =true;
                                
                                }
                            else if(entered_key =='D'){
                                   lcd.clear();
                                   lcd.setCursor(0,0);
                                   switch(indicator){
                                      case 'u':
                                          lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("Enter maximum");
                                           lcd.setCursor(0,1);
                                           lcd.print("level:");
                                          break;
                                      case 'l':
                                          lcd.clear();
                                          lcd.setCursor(0,0);
                                          lcd.print("Enter minimum");
                                          lcd.setCursor(0,1);
                                          lcd.print("level:");
                                          break;    
                                    }
                                   entered_key_count =0; 
                              }
                     }
                }
        return intArrayToValue(setpoint_type); 
  }

int to_percentage(int raw){
   return  100*(tank_height-raw)/tank_allowed_maximum_level;
  }

 void user_login(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("<Enter password>");
    byte entered_key_count =0;
    while(entered_key_count<user1.user_password_length){
        char entered_key =keypad.getKey();
        if(entered_key){
              switch(entered_key){
                  case 'A':
                      return ;
                  case 'D':
                      reset_display("Enter password:",0,0);
                      entered_key_count = 0;
                  default:
                      user1.password[entered_key_count] = entered_key;
                      entered_key_count +=1;
              }
         }
     }
  }
  
struct user get_user_password(byte row, byte column){ 
      byte entered_key_count=0;
      struct user local_user;
      while(entered_key_count<user1.user_password_length){
             char entered_key =keypad.getKey();
             if(entered_key){ 
                  switch(entered_key){
                        case 'A':
                            local_user.password =user1.password;
                            return local_user;
                        case 'D': // delete entered keys
                            reset_display("Enter password:",0,0);
                            entered_key_count = 0;
                            break;
                        default:
                            local_user.password[entered_key_count] = entered_key;
                            lcd.setCursor(column+entered_key_count,row);
                            lcd.print("*");
                            entered_key_count +=1;                     
              }
           }
         
       }
      return local_user;
}
 
void reset_display(String new_message, byte message_row, byte message_column){
      lcd.clear();
      lcd.setCursor(message_column,message_row);
      lcd.print(new_message);
  }
 void print_info(String msg){
    lcd.clear();
    for(int index =0; index<msg.length(); index++){
       if(msg[index] !=',' && index<16){
          lcd.setCursor(index, 0);
          lcd.print(msg[index]);
        }
        else if(msg[index] !=',' && index<32){
          lcd.setCursor(index-16, 1);
          lcd.print(msg[index]);
        }
        else if(msg[index] !=',' && index<48){
          lcd.setCursor(index, 0);
          lcd.print(msg[index]);
        }
       else{
          lcd.setCursor(index,1);
          lcd.print(msg[index]);
        }
     }
     delay(2000);  
     lcd.clear(); 
  }
 void change_password(){
       byte attempt =0;
       byte attempt_limit =3;
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("You need to");
             lcd.setCursor(0,1);
             lcd.print("enter the old ");  
             lcd.setCursor(20,0);
             lcd.print("password!");
             delay(2000); 
      label1: 
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Old password:");
              struct user local_user =get_user_password(1,6);
              if(local_user.password.equals(custom_eeprom_read(4))){
                        attempt=0;
                  label2:
                         attempt+=1;
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("New password:");
                         lcd.setCursor(17,1);
                         lcd.print("Attempt:"+String(attempt)+" of "+String(attempt_limit));
                         struct user local_user2 = get_user_password(1,6);
                         if(local_user2.password.equals(custom_eeprom_read(4))){
                             lcd.clear();
                             lcd.setCursor(0,0);
                             lcd.print("New password");
                             lcd.setCursor(0,1);
                             lcd.print("should not be");  
                             lcd.setCursor(16,0);
                             lcd.print("the same as!");
                             lcd.setCursor(16,1);
                             lcd.print("old password!");
                             delay(2000); 
                             if(attempt == attempt_limit){
                                return;
                              }
                              else{
                                goto label2;
                              }
                          }
                         else{
                            custom_eeprom_write(local_user2.password);
                             lcd.clear();
                             lcd.setCursor(0,0);
                             lcd.print("New password");
                             lcd.setCursor(0,1);
                             lcd.print("successfully");  
                             lcd.setCursor(21,0);
                             lcd.print("saved!");
                             delay(2000); 
                             success_beef();
                             return;
                          }
                }
               else{
                   attempt+=1;
                   lcd.clear();
                   lcd.setCursor(0,0);
                   lcd.print("Wrong passowrd!");
                   lcd.setCursor(0,1);
                   lcd.print("Try again");
                   lcd.setCursor(17,1);
                   lcd.print("Attempt:"+String(attempt)+" of "+String(attempt_limit));
                   error_beef();
                   delay(500);
                  if(attempt != attempt_limit){
                      //print_info("Wrong password!");
                      goto label1;
                    }
                  else{
                       return;
                    }  
                }
  }

 void custom_eeprom_write(String data){
      for (int i =0; i<data.length(); i++){
         EEPROM.write(all_addresses.password_address[i],data[i] );
         }
    }
String custom_eeprom_read(byte address_length){
      String string_buffer ="0000";
      for(int i =0; i<address_length; i++){
           string_buffer[i] =EEPROM.read(all_addresses.password_address[i]);   
        }
        return string_buffer;
  }
 void custom_eeprom_setpoints_update(String data){
      for (int i =0; i<data.length(); i++){
         EEPROM.update(all_addresses.password_address[i],data[i] );
         }
    }

void show_tank_parameters(){
  for(byte i = 0; i < 4; i++){
    switch(i){
        case 0:
           lcd.setCursor(0, 0);
           lcd.print("Height(cm):");
           delay(1000);  
           break;   
        case 1:
           lcd.setCursor(6, 1);
           lcd.print(EEPROM.read(all_addresses.tank_height_address)); 
           delay(1000); 
           break;
        case 2:
           lcd.setCursor(17,0);
           lcd.print("Diameter(cm):");
           delay(1000);
           break;
        case 3:
           lcd.setCursor(23,1);
           lcd.print(EEPROM.read(all_addresses.tank_diameter_address));
           delay(2500);
           break;
        }
    }
  }
void manufacturer_details(){
  for(int i =0; i<4; i++){
      struct manufacturer manuf;
      switch(i){
        case 0:
           lcd.setCursor(0,0);
           lcd.print(manuf._name);
           delay(1000);  
           break;   
        case 1:
           lcd.setCursor(3,1);
           lcd.print(manuf._date); 
           delay(1000); 
           break;
        case 2:
           lcd.setCursor(19,0);
           lcd.print(manuf._phone);
           delay(1000);
           break;
        case 3:
           lcd.setCursor(17,1);
           lcd.print(manuf._website);
           delay(2500);
           break;
        }
    }
 }



unsigned long volume(int height, int diameter){
  return 3.142*height*diameter*diameter/4;
  
  }

int get_distance(){
  return sensor1.ping_cm();
  }

