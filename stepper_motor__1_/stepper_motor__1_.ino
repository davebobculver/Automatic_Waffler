/*
  Stepper Motor Control — using <Stepper.h>
  ==========================================
  Wiring (4-wire bipolar/unipolar stepper via L298N or ULN2003):
    IN1 → Arduino pin 8
    IN2 → Arduino pin 9
    IN3 → Arduino pin 10
    IN4 → Arduino pin 11
    Motor power supply → driver board VCC (5–12 V depending on motor)
    GND → common ground with Arduino

  Adjust STEPS_PER_REV to match your motor (common values: 32, 48, 200, 2048).
  For 28BYJ-48 with ULN2003 driver: STEPS_PER_REV = 2048 (half-step mode)
  For a typical NEMA17 (1.8°/step): STEPS_PER_REV = 200
*/

#include <Stepper.h>

int out= 4;

const int STEPS_PER_REV = 200;  
const int MOTOR_SPEED   = 60;    

Stepper myStepper(STEPS_PER_REV, 8, 9, 10, 11);

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(MOTOR_SPEED);
  Serial.println("Stepper motor ready.");
  pinMode(out, OUTPUT);
}

void loop() {

  myStepper.step(STEPS_PER_REV);
  delay(1000);               

digitalWrite(out,HIGH);
delay(1000);
digitalWrite(out,LOW);
delay(1000);



}
