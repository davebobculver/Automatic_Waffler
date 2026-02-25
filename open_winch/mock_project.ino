//***SERVO***//
#include <Servo.h>
int flag = 0;
Servo hitec; //Instantiate Servo
const int ServoPin = 9;

//***PHOTODIODES***//
int const photodiode1 = A0;
int const photodiode2 = A1;

//***IR proxy***//
int const InfraRedPin = 2;

//***SETUP***//
void setup() {

  //Attach Servo to Pin9
    hitec.attach(ServoPin, 550, 2800);

  //Inputs to Photodiodes
    pinMode(photodiode1, INPUT);
    pinMode(photodiode2, INPUT);

  //IR proxy sensor
    pinMode(InfraRedPin, INPUT);

  //Attach Interrupt (STILL NEEDS A METHOD)
  //attachInterrupt(0, get_keypress, FALLING);
  
  //Start monitor and print Ready!
    Serial.begin(9600);
    Serial.println("Ready!");

}

void loop() {

  if (flag == 0) {
    hitec.write(0);        // send servo to 0 degrees
    flag = 1;
  }
  else {
    hitec.write(180);      // send servo to 180 degrees
    flag = 0;
  }


  delay(1000);
}
