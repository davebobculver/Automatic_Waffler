//PHYS 124 PROJECT: AUTOMATIC WAFFLER
//David (the Goat) Culver, Johnny Lemus
/*--------------------------------------------------------------------------*/

//Libraries and StateMachine Setup w/Initialization

  //Libraries
  #include <math.h>
  #include <Stepper.h>        
  #include <LiquidCrystal.h> //FOR LCD

  #include "MyLibrary.h"

  //Force Sensor
    #include <HX711_ADC.h>
    #if defined(ESP8266)|| defined(ESP32) || defined(AVR)
    #include <EEPROM.h>
    #endif
    
    enum SystemState{
      INIT,             //Initialize all systems
      CALIBRATE,        //User Calibrates the Scale
      SCALE_TEST,       //Test if Scale Works
      IDLE,             //Standby for User Input
      READY_FOR_BATTER, //Wait for Batter
      DISPENSE,         //Dispense Batter(after ready)
      READY_TO_COOK,    //Check if Ready to Cook
      COOKING,          //Start cooking (timer + temp + etc)
      COOKED,           //Finish cook
      COMPLETE,         //Show Waffle is complete (if time allows try to LCD)
      ERROR             //Error should stop all processes immediately
    }

  currentState = INIT; //Start system 

  //Stepper
  const int stepsPerRevolution = 200; // Number of steps per revolution 
  Stepper myStepper(stepsPerRevolution, MOTORPIN1 , MOTORPIN2, MOTORPIN3, MOTORPIN4); // Pins connected to the motor
  const int numberofSteps = 1000;
/*--------------------------------------------------------------------------*/
//Layout/Controls + Setup

  //PINS

    //PUSHBUTTON:
      const int startButton = 2;
    
    //SOLENOID
     const int solenoidPin = 3;

    //HX711
     const int HX711_dout = 6; //mcu > HX711 dout pin
     const int HX711_sck = 7; //mcu > HX711 sck pin
     HX711_ADC LoadCell(HX711_dout, HX711_sck); 
     const int calVal_eepromAdress = 0;

    //MOTOR SHIELD
    const int MOTORPIN1 = 8;
    const int MOTORPIN2 = 9;
    const int MOTORPIN3 = 10;
    const int MOTORPIN4 = 11;

  //GLOBAL VARIABLES
  float currentForce = 0; 
  float batterTargetWeight = 500;
  unsigned long cookStartTime;
  bool waffleOpen = false;
  unsigned long dispenseTime = 3000; //MUST BE CHANGED
  /*--------------------------------------------------------------------------*/
  // Hardware abstraction and calibration function implementations are now
  // located in MyLibrary.cpp with their declarations in MyLibrary.h.

/*--------------------------------------------------------------------------*/


//Setup
  void setup(){
    
  Serial.begin(57600);
  delay(10);
  Serial.println("System starting...");
  
  //Stepper
    myStepper.setSpeed(60);
      
  //PUSHBUTTON
  pinMode(startButtonPin, INPUT_PULLUP);

  // SOLENOID
    pinMode(solenoidPin, OUTPUT);
    digitalWrite(solenoidPin, LOW);
    
  //HX711 + LoadCell Initiate
  LoadCell.begin();
    unsigned long stabilizingtime = 2000;
    boolean _tare = true;
  
    LoadCell.start(stabilizingtime, _tare);
  
    if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
      Serial.println("HX711 Timeout - Check Wiring");
      currentState = ERROR;
    }
    else {
      Serial.println("HX711 Startup Complete");
  }

  // Load saved calibration factor
    float savedCalFactor;
    EEPROM.get(calVal_eepromAdress, savedCalFactor);
    
    // reject bad values
    if (isnan(savedCalFactor) || savedCalFactor == 0) {
    
      Serial.println("No valid calibration factor found.");
      Serial.println("Using default calibration.");
    
      savedCalFactor = 1.0;
    }   
    
    LoadCell.setCalFactor(savedCalFactor);
    
    Serial.print("Calibration Factor: ");
    Serial.println(savedCalFactor);
  }
/*--------------------------------------------------------------------------*/
//Main Loop
void loop() {

  currentForce = readForce();

  switch (currentState) {

    case INIT: {

      Serial.println("Initializing.....");
      delay(1000);
      
      Serial.println("Entering Calibration Mode...");
      currentState = CALIBRATE;

    break;
    }

    
    case CALIBRATE: {
      calibrate();
      currentState = SCALE_TEST;
      }
    break;
    
  case SCALE_TEST: 

  static unsigned long lastPrint = 0;

  LoadCell.update();

  if(millis() - lastPrint > 500){

    float force = readForce();

    Serial.print("Force: ");
    Serial.print(force);
    Serial.println(" grams");

    lastPrint = millis();
  }

  if(Serial.available()){

    char cmd = Serial.read();

    if(cmd == 's'){
      Serial.println("Scale test complete.");
      currentState = IDLE;
    }

    if(cmd == 't'){
      Serial.println("Re-taring scale...");
      LoadCell.tareNoDelay();
    }

  }

  break;

    case IDLE:

      Serial.println("Idle: Waiting for Start");

      //Open Waffler if button 
      if(digitalRead(startButtonPin) == LOW){
        Serial.println("Button pressed. Opening Waffler...");
        openWaffleIron();
        currentState = READY_FOR_BATTER;
      }
      delay(500);
      break;
    
    case READY_FOR_BATTER:

      //Check if the Waffler is open, then opens the valve for batter to dispense
      if(waffleIronIsOpen()){ //returns 0 if closed, 1 if waffleIronIsOpen()
        Serial.println("Waffler Open. Dispensing Batter...");
        openValve(); //Dispense Batter
        currentState = DISPENSE; //Break with no delay to ensure smooth dispense
        break;
      }

      delay(500);
      break;

    case DISPENSE:
{
    float force = readForce(); // continuously read currentForce

    Serial.print("Force: ");
    Serial.println(force);

    // If batter has reached target weight
    if(force >= batterTargetWeight)
    {
      closeValve();
      closeWaffleIron();
      currentState = READY_TO_COOK;
    }

    break;
  }
  
  case READY_TO_COOK:
    Serial.println("Ready to Cook!");

    //Set timer
    cookStartTime = millis();
    currentState = COOKING;
    delay(20);
    break;
  
  case COOKING:
    Serial.println("We are cooking your delicious waffle...");
    delay(20);
    //Maybe add a fun fact about waffles?

    //Check if waffle has been cooked too long
    if (millis() - cookStartTime > 60000*5){   
      Serial.println("ERROR: MAX COOKING TIME EXCEEDED");
      currentState = ERROR;
    }

    //Check if Waffle is done cooking
    //CHECK THESE CONDITIONS manually???
    if(millis() - cookStartTime > 60000*4){
      currentState = COOKED;
      break;
    }

  delay(500);
  break;
  

  case COMPLETE:

      //Show completedness 
      //Maybe add LCD display showing waffle count, how heavy waffle was etc...
      Serial.println("All Done");
      break;
      return;

    case ERROR:
      //SystemShutdown();
      return;
  }
}
