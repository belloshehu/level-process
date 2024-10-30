//pins used for keypad connection
const byte keypadRow1 =0;
const byte keypadRow2 =1;
const byte keypadRow3 =2;
const byte keypadRow4 =3;
const byte keypadCol1 =4;
const byte keypadCol2 =7;
const byte keypadCol3 =6;
const byte keypadCol4 =5;
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

byte rowPins[ROWS] = {keypadRow1,keypadRow2,keypadRow3,keypadRow4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {keypadCol1,keypadCol2,keypadCol3,keypadCol4}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

//end of keypad configuration
