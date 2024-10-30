
int tank_allowed_maximum_level =0; //190 centimeters
int tank_diameter =0;
int tank_height =0; //200 centimeters

struct addresses{
    byte upper_setpoint_address=6;
    byte lower_setpoint_address=7;
    byte password_address[5]={1,2,3,4};
    byte tank_height_address = 8;
    byte tank_diameter_address =10;
  }all_addresses ;
  
struct tank_parameters{
    byte height=0;
    byte diameter =0;
  };

bool save_tank_parameter(unsigned short height, unsigned short diameter){
      if(height == 0 && diameter == 0 ){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Entered value");
              lcd.setCursor(0,1);
              lcd.print("is incorrect.");
              lcd.setCursor(21,0);
              lcd.print("Try again!");
              return false;    
        }
       else if(height > 0 && diameter == 0){
           EEPROM.update(all_addresses.tank_height_address,height); 
           return true;
        }
       else if(height == 0 && diameter > 0){
          EEPROM.update(all_addresses.tank_diameter_address,diameter); 
          return true;
        }
       else{
          EEPROM.update(all_addresses.tank_height_address,height); 
          EEPROM.update(all_addresses.tank_diameter_address,diameter); 
          return true;
       }
  }

int  intArrayToValue2(byte intArray[]){
    int val=0;
    for(int key =2; key >=0; key--){
            int decimal_value =pow(10,key);
            byte arrayElement = intArray[2-key];
            val = val+arrayElement*decimal_value;     
      }
     return val;
    }

int charToInt(char key){
            switch(key){
              case '0':
              return 0;
              break;
              case '1':
              return 1;
              case '2':
              return 2;
              break;
              case '3':
              return 3;
              case '4':
              return 4;
              break;
              case '5':
              return 5;
                case '6':
              return 6;
              break;
              case '7':
              return 7;
                case '8':
              return 8;
              break;
              case '9':
              return 9;
              }
    }

struct tank_parameters get_tank_parameters(char parameter_type){
      struct tank_parameters my_tank;
      byte entered_key_count=0;
      const byte tank_diameter_digits =3;
      const byte tank_height_digits =3;
      char tank_height_selection_key ='A';
      char tank_diameter_selection_key ='B';
      bool setpoint_done =false;
      static byte t_height[3];
      static byte t_diameter[3];
      while(entered_key_count < 3){
                    char entered_key =keypad.getKey();
                    if(entered_key){
                                if(isDigit(entered_key)){
                                   switch(parameter_type){
                                      case 'h':
                                         t_height[entered_key_count] =charToInt(entered_key);
                                         lcd.setCursor(23+entered_key_count,0);
                                         lcd.print(entered_key);
                                         entered_key_count+=1;
                                         break;
                                      case 'd':
                                         t_diameter[entered_key_count] =charToInt(entered_key);
                                         lcd.setCursor(23+entered_key_count,0);
                                         lcd.print(entered_key);
                                         entered_key_count+=1;
                                         break;
                                    }
                                  }
                                //point_done =true;
                         
                               else if(entered_key =='D'){ // press 'D' to delete 
                                   entered_key_count =0; 
                                   if(parameter_type == 'h'){
                                      lcd.clear();
                                      lcd.setCursor(0,0);
                                      lcd.print("Enter tank");
                                      lcd.setCursor(0,1);
                                      lcd.print("height in cm:");
                                    }
                                    else{
                                      lcd.clear();
                                      lcd.setCursor(0,0);
                                      lcd.print("Enter tank");
                                      lcd.setCursor(0,1);
                                      lcd.print("diameter in cm:");
                                      }
                               }
                     }
                }
        my_tank.height =intArrayToValue2(t_height);
        my_tank.diameter =intArrayToValue2(t_diameter);
        return my_tank;
  }

void set_tank_parameters(){
    again:  lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Press A to set");
      lcd.setCursor(2,1);
      lcd.print("tank height.");
      lcd.setCursor(16,0);
      lcd.print("Press B to set");
      lcd.setCursor(17,1);
      lcd.print(" tank diameter");
      bool work_done =false;
      byte t_height =0;
      byte t_diameter =0; 
      static struct tank_parameters user_tank;
      while(work_done == false){
                    char entered_key = keypad.getKey();
                    if(entered_key){
                           switch(entered_key){
                              case 'A':
                                    lcd.clear();
                                    lcd.setCursor(0,0);
                                    lcd.print("Enter tank");
                                    lcd.setCursor(0,1);
                                    lcd.print("height in cm:");
                                    t_height = get_tank_parameters('h').height;
                                    break;
                              case 'B':
                                    lcd.clear();
                                    lcd.setCursor(0,0);
                                    lcd.print("Enter tank");
                                    lcd.setCursor(0,1);
                                    lcd.print("diameter in cm:");
                                    t_diameter = get_tank_parameters('d').diameter;
                                    break;
                              case '*': // save configuration
                                    bool result = save_tank_parameter(t_height, t_diameter);
                                    tank_height = t_height;
                                    tank_allowed_maximum_level = tank_height -15;
                                    tank_diameter = t_diameter;
                                    if(result){
                                      lcd.clear();
                                      lcd.setCursor(0,0);
                                      lcd.print("Height => "+String(t_height));
                                      lcd.setCursor(0,1);
                                      lcd.print("Diameter=> "+String(t_diameter)+"cm");
                                      lcd.setCursor(17,0);
                                      lcd.print("successfully");
                                      lcd.setCursor(22,1);
                                      lcd.print("Save");
                                      delay(3000);
                                      work_done = true;
                                      return;
                                     }
                                     else{
                                      lcd.clear();
                                      lcd.setCursor(0,1);
                                      lcd.print("Failed to save!");
                                      lcd.setCursor(0,2);
                                      lcd.print(t_height);
                                      lcd.setCursor(9,2);
                                      lcd.print(t_diameter);
                                      delay(3000);
                                       goto again;
                                      }
                                      break;
                            }
                       }
              }
  }
  
bool initialise_tank_parameters(){
   //struct tank_parameters tp;
   byte height = EEPROM.read(all_addresses.tank_height_address);
   tank_height = height;
   tank_allowed_maximum_level = tank_height - 20;
   tank_diameter = EEPROM.read(all_addresses.tank_diameter_address);
   if( height == 0 || height == 255){
      return false;
    }
    else{
      tank_allowed_maximum_level = tank_height - 20;
      return true;
    }
  }
  
int percent_cm(float percentage){
    int content_height = tank_height - percentage*tank_allowed_maximum_level/100;
    return content_height;
   }
    
