#define in1 8
#define in2 9
#define in3 10
#define in4 11

void setup() {
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);
}

void loop(){

step_cw();
delay(3000);
// digitalWrite(in1, LOW);
// digitalWrite(in2, LOW);
// digitalWrite(in3, HIGH);
// digitalWrite(in4, LOW);

}

void step(int num_step, int dir, int speed){

  if (dir == 0){
    for (int j = 0; j < num_step; j++){
      step_cw();
      delay(speed);
    }
  }

  if (dir == 1){
    for (int j = 0; j < num_step; j++){
      step_ccw();
      delay(speed);
    }
  }

}

void step_ccw(){

digitalWrite(in4, HIGH);
digitalWrite(in2, LOW);
digitalWrite(in1, LOW);
digitalWrite(in3, LOW);
delay(1000);

digitalWrite(in2, HIGH);
digitalWrite(in3, LOW);
digitalWrite(in4, LOW);
digitalWrite(in1, LOW);
delay(1000);

digitalWrite(in3, HIGH);
digitalWrite(in2, LOW);
digitalWrite(in1, LOW);
digitalWrite(in4, LOW);
delay(1000);

digitalWrite(in1, HIGH);
digitalWrite(in2, LOW);
digitalWrite(in4, LOW);
digitalWrite(in3, LOW);
delay(1000);

}

void step_cw(){

digitalWrite(in4, HIGH);
digitalWrite(in3, LOW);
digitalWrite(in2, LOW);
digitalWrite(in1, LOW);
delay(1000);

digitalWrite(in1, HIGH);
digitalWrite(in4, LOW);
digitalWrite(in3, LOW);
digitalWrite(in2, LOW);
delay(1000);

digitalWrite(in3, HIGH);
digitalWrite(in2, LOW);
digitalWrite(in4, LOW);
digitalWrite(in1, LOW);
delay(1);

digitalWrite(in2, HIGH);
digitalWrite(in1, LOW);
digitalWrite(in3, LOW);
digitalWrite(in4, LOW);
delay(1000);

}