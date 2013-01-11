
/* 

by Costyn van Dongen 

Stepper code examples by Tom Igoe 

Future feature ideas:
* Reset button to bring everything to starting point
* Potmeters/encoders to increase/decrease stepper speed and delay that the camera takes a picture

 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
                                     // for your motor
const int stepsSize = 200;  // change this to fit the number of steps per revolution

const int relayPin = 5;  // change this to fit the number of steps per revolution
const int bulbTime = 1000;  // number of milliseconds the shutter should be open

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

int stepCount = 0;         // number of steps the motor has taken

void setup() {
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  // step one step:
  myStepper.step(stepSize);
  stepCount++;
  Serial.print("steps:" );
  Serial.println(stepCount);
  delay(1000); // let the rig settle
  digitalWrite(relayPin, HIGH); // close the relay, open the shutter
  delay(bulbTime);  // bulb mode 
  digitalWrite(relayPin, LOW); // open the relay, close the shutter
  delay(500) ; // wait for a bit
}

