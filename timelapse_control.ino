#include <LiquidCrystal.h>
#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution for your motor

Stepper myStepper(stepsPerRevolution, A1,A2,A3,A4);            
int stepCount = 0;         // number of steps the motor has taken

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 7, 6);

//From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/

//these pins can not be changed 2/3 are special pins
const int encoderPin1 = 2;
const int encoderPin2 = 3;
const int encoderSwitchPin = 8; //push button switch
const int relayPin = A0 ;

volatile int lastEncoded = 0;
volatile long encoderValue1 = 0;
volatile long encoderValue2 = 0;
volatile long shutterTime = 0;
volatile long stepperSpeed = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

int whichValue = 1 ;

const int maxDelay = 30 ;
const int maxSpeed = 10 ;

unsigned long time;
unsigned long lastShutterTime;

void setup() {  
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
  pinMode(encoderSwitchPin, INPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  digitalWrite(encoderSwitchPin, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
//  lcd.print("hello, world!");
  delay(1000);
}

void loop() {
  // scroll 13 positions (string length) to the left 
  // to move it offscreen left:
  lcd.setCursor(0, 0); // top left
  lcd.print( "Shutter: " ) ;
  lcd.print(shutterTime, DEC) ;
  lcd.print(" ");
  lcd.setCursor(0, 1); // bottom left
  lcd.print( "  Speed: " ) ;
  lcd.print(stepperSpeed, DEC) ;
  lcd.print(" ");

  if(digitalRead(encoderSwitchPin) == LOW ){
    if( whichValue == 1 ) {
      whichValue = 2 ;
      Serial.print("whichvalue = ") ;
      Serial.println(whichValue) ;
    } else {
      whichValue = 1 ;
      Serial.print("whichvalue = ") ;
      Serial.println(whichValue) ;
    }
    while(digitalRead(encoderSwitchPin) == LOW) { delay(10) ; } // idle while waiting for button to be released
  }else{
//    Serial.println("no button press");
  }

  // delay at the end of the full loop:
  delay(100);
  
  time = millis();
//  Serial.println(time) ;

  // current time + shutter
  if( lastShutterTime + 2*(shutterTime*1000) < time  ) {
     digitalWrite( relayPin, HIGH ) ;
     lastShutterTime = time ;
     Serial.print("lastShutterTime = ");
     Serial.print(lastShutterTime);
     Serial.print("  shutterOffTime = ");
     Serial.println(time + (shutterTime*1000));
  }
  
  if( time - lastShutterTime  > (shutterTime*1000) ) {
     digitalWrite( relayPin, LOW ) ;    
  }
  
  myStepper.step(stepperSpeed);
  stepCount++;
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) { 
    if( whichValue == 1 ) {
      encoderValue1++ ; 
    } else {
      encoderValue2++ ;
    }
  }
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) { 
    if( whichValue == 1 ) {
      encoderValue1-- ; 
    } else {
      encoderValue2-- ;
    }
  }

  lastEncoded = encoded; //store this value for next time
  shutterTime = (abs(encoderValue1) / 4)%30 ;
  stepperSpeed = (abs(encoderValue2) / 4)%30 ;
}
