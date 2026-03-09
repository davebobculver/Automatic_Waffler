//PHYS 124 PROJECT: AUTOMATIC WAFFLER
//David (the Goat) Culver, Johnny Lemus
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//Libraries and StateMachine Setup w/Initialization

  //Libraries
  #include <math.h>
  #include <Servo.h>          
  #include <Stepper.h>        
  #include <LiquidCrystal.h> 

  #include "MyLibrary.h"

  //Force Sensor
    #include <HX711_ADC.h>
    #if defined(ESP8266)|| defined(ESP32) || defined(AVR)
    #include <EEPROM.h>
    #endif
    
    enum SystemState{
      INIT,             //Initialize all systems
      CALIBRATE,
      SCALE_TEST,
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
  const int stepsPerRevolution = 200; // Number of steps per revolution (NEED TO CHECK)
  Stepper myStepper(stepsPerRevolution, 10,11, 12, 13); // Pins connected to the motor
  const int updownsteps = 60;
  bool flag = 0;
/*--------------------------------------------------------------------------*/
//Layout/Controls + Setup

  //PIN LAYOUT
    //5 Digital Pins (TO BE CHANGED)
     const int startButtonPin = 2;
     const int HX711_dout = 4; //mcu > HX711 dout pin
     const int HX711_sck = 5; //mcu > HX711 sck pin
     const int solenoidPin = 6;       // Arduino pin controlling L298N for solenoid
     const int solenoidPinPair = 7;   // Keep this LOW for one-way operation

    //1 Analog Pin(s) (TO BE CHANGED)
     const int forceSensorPin = A3;
     
  //Timers for cooking
    long cookStartTime = 0;

  //Controls
    int WaffleCount = 0; 
    int desiredWaffles = 3; //MAYBE WE CAN CHANGE THIS VIA USER INPUT with PUSHBUTTON

  /*--------------------------------------------------------------------------*/
  // Hardware abstraction and calibration function implementations are now
  // located in MyLibrary.cpp with their declarations in MyLibrary.h.
    
      if(!started){
        Serial.println("\n=== SCALE CALIBRATION ===");
        Serial.println("Remove all weight from scale.");
        Serial.println("Press 't' to tare.");
        started = true;
      }
    
      LoadCell.update();
    
      if(Serial.available()){
        char cmd = Serial.read();
    
        if(cmd == 't'){
          Serial.println("Taring...");
          LoadCell.tareNoDelay();
        }
      }
    
      if(LoadCell.getTareStatus()){
    
        Serial.println("Tare complete.");
        Serial.println("Place known mass on scale.");
        Serial.println("Enter weight in grams:");
    
        float knownMass = 0;
    
        while(knownMass == 0){
    
          LoadCell.update();
    
          if(Serial.available()){
            knownMass = Serial.parseFloat();
          }
        }
    
        Serial.print("Entered mass: ");
        Serial.println(knownMass);
    
        LoadCell.refreshDataSet();
    
        float newCalFactor = LoadCell.getNewCalibration(knownMass);
    
        Serial.print("New calibration factor: ");
        Serial.println(newCalFactor);
    
        EEPROM.put(calVal_eepromAdress, newCalFactor);
        LoadCell.setCalFactor(newCalFactor);
    
        Serial.println("Calibration saved.");
    
        started = false;
        currentState = SCALE_TEST;
      }

  break;
}

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
      if(digitalRead(startButtonPin) ==LOW){
        Serial.println("Button pressed. Opening Waffler...");
        openWaffleIron();
        currentState = READY_FOR_BATTER;
      }
      delay(500);
      break;
    
    case READY_FOR_BATTER:

      //Check if the Waffler is open, then opens the valve for batter to dispense
      if(waffleIronisOpen()){ //returns 0 if closed, 1 if waffleIronIsOpen()
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
        Serial.println("Target weight reached! Opening solenoid...");
        digitalWrite(solenoidPin, HIGH);  // Open valve
        unsigned long dispenseStart = millis();

        // Keep solenoid open for dispenseTime
        while(millis() - dispenseStart < dispenseTime)
        {
            LoadCell.update();                  // continue updating load cell
            currentForce = readForce();         // update global variable
            Serial.print("Force during dispense: ");
            Serial.println(currentForce);
            delay(50);                          // small delay to avoid flooding serial
        }

        digitalWrite(solenoidPin, LOW);        // Close valve
        Serial.println("Solenoid closed. Dispense complete.");

        // Close waffler
        closeWaffleIron();
        currentState = READY_TO_COOK;
    }
    else
    {
        // Optional: keep showing force if below threshold
        Serial.println("Waiting for batter...");
        delay(200);
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
  /*--------------------------------------------------------------------------*/
  // Hardware abstraction and calibration function implementations are now
  // located in MyLibrary.cpp with their declarations in MyLibrary.h.
