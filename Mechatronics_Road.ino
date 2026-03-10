//Pin Definitions
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
//Road 1 LED's
const int R1_LRed = 2;
const int R1_LAmber = 3;
const int R1_LGreen = 4;
//Road 2 LED's
const int R2_LRed = 5;
const int R2_LAmber = 6;
const int R2_LGreen = 7;
//Pedestrian Crossing LED's
const int Ped_Green = 11;
const int Ped_Red= 12;
//inputs
const int PIN_BTN= A1;

//Requests
bool PedestrianRequest= false;

//Machine
enum State {
R1_Green, R1_Amber, R1_Red,
R2_Green, R2_Amber, R2_Red,
R1_RedAmber, 
R2_RedAmber,
Pedestrian_Walk, Pedestrian_Clear
};

State state = R1_Green;
unsigned long t0 = 0;

//Time
const unsigned long T_Green_1 = 8000;
const unsigned long T_Green_3 = 5000;
const unsigned long T_Amber = 3000;
const unsigned long T_Red = 1000;
const unsigned long T_Red_Amber = 1000;
const unsigned long T_Pedestrian_Walk = 8000;
const unsigned long T_Pedestrian_Clear= 2000;

//Function

void setupIO();
void readInputs();
void AllRed();
void SetA(int r, int a, int g);
void SetB(int r, int a, int g);
void Set_Pedestrian(bool walkOn);
void enter(State s);
bool elapsed(unsigned long ms);

void setup() {
setupIO();
lcd.init();
lcd.backlight();
lcd.clear();
lcd.setCursor(0,0);
lcd.print("PEDESTRIAN");
lcd.setCursor(0,1);
lcd.print("WAIT");

enter(R1_Green);
}

void loop() {
readInputs();
switch (state) {

  case R1_Green:
  if (elapsed(T_Green_1)) enter(R1_Amber);
  break;

  case R1_Amber:
  if (elapsed(T_Amber)) enter(R1_Red);
  break;

  case R1_Red:
  if (elapsed(T_Red)){
    if (PedestrianRequest) enter(Pedestrian_Walk);
    else enter(R2_RedAmber);
  }
  break;

  case R2_Green:
  if (elapsed(T_Green_1)) enter(R2_Amber);
  break;

  case R2_Amber:
  if (elapsed(T_Amber)) enter(R2_Red);
  break;

  case R2_Red:
   if (elapsed(T_Red)){
    if (PedestrianRequest) enter(Pedestrian_Walk);
    else enter(R1_RedAmber);
   }
  break;

case R1_RedAmber:
if (elapsed(T_Red_Amber))enter(R1_Green);
break;

case R2_RedAmber:
if (elapsed(T_Red_Amber))enter(R2_Green);
break;



 case Pedestrian_Walk:
 if(elapsed(T_Pedestrian_Walk)) enter(Pedestrian_Clear);
 break;

 case Pedestrian_Clear:
 if (elapsed(T_Pedestrian_Clear)) {
  PedestrianRequest = false;
  enter(R1_Green);
 }
break;
}
}

//Hardware
void setupIO() {
  pinMode(R1_LRed, OUTPUT); pinMode(R1_LAmber, OUTPUT); pinMode(R1_LGreen, OUTPUT);
  pinMode(R2_LRed, OUTPUT); pinMode(R2_LAmber, OUTPUT); pinMode(R2_LGreen, OUTPUT);
  pinMode(Ped_Green, OUTPUT); pinMode(Ped_Red, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);

  AllRed();
  Set_Pedestrian(false);
}

//Inputs
void readInputs() {
  static int lastStable = HIGH;
  static int lastReading = HIGH;
  static unsigned long lastChanged = 0;

  int reading = digitalRead(PIN_BTN);

  if (reading != lastReading) {
    lastChanged = millis();
    lastReading = reading;
  }

  if(millis() - lastChanged > 30) {
    if(reading != lastStable){
      lastStable = reading;
      if (lastStable == LOW) {
        PedestrianRequest = true;
      }
    }
  }
}

//State Control
void enter(State s){
  state = s;
  t0 = millis();
  AllRed();
  Set_Pedestrian(false);
  updateLCDForState(state);

  switch (state) {

    case R1_Green: SetA(0,0,1); break;
    case R1_Amber: SetA(0,1,0); break;
    case R1_Red: SetA(1,0,0); break;

    case R2_Green: SetB(0,0,1); break;
    case R2_Amber: SetB(0,1,0); break;
    case R2_Red: SetB(1,0,0); break;

    case Pedestrian_Walk: 
    Set_Pedestrian(true);
     break;
    default: break;

    case R1_RedAmber: SetA(1,1,0); break;
    case R2_RedAmber: SetB(1,1,0); break;
  }
}

void updateLCDForState(State s){
  lcd.clear();
lcd.setCursor(0,0);
lcd.print("PEDESTRIAN");

lcd.setCursor(0,1);

if (s== Pedestrian_Walk){
  lcd.print("WALK");
} else if (PedestrianRequest) {
  lcd.print("WAITING...");
} else{
  lcd.print("WAIT");
}
}

bool elapsed(unsigned long ms) {
  return millis() - t0 >= ms;
}

//Ouputs
void AllRed() {
  digitalWrite(R1_LRed, HIGH); digitalWrite(R1_LAmber, LOW); digitalWrite(R1_LGreen, LOW);
  digitalWrite(R2_LRed, HIGH); digitalWrite(R2_LAmber, LOW); digitalWrite(R2_LGreen, LOW);
}

void SetA (int r, int a, int g) {
  digitalWrite(R1_LRed, r ? HIGH : LOW);
  digitalWrite(R1_LAmber, a ? HIGH : LOW);
  digitalWrite(R1_LGreen, g ? HIGH : LOW);
}

void SetB (int r, int a, int g) {
  digitalWrite(R2_LRed, r ? HIGH : LOW);
  digitalWrite(R2_LAmber, a ? HIGH : LOW);
  digitalWrite(R2_LGreen, g ? HIGH : LOW);
}

void Set_Pedestrian(bool walkOn) {
  digitalWrite(Ped_Green, walkOn ? HIGH : LOW);
  digitalWrite(Ped_Red, walkOn ? LOW : HIGH);
}


