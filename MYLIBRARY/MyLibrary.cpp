// Contains implementations for the hardware abstraction and calibration
// functions declared in MyLibrary.h.

#include "MyLibrary.h"
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Stepper.h>

//EXTERNS VARIABLES
extern const int solenoidPin;
extern Stepper myStepper;
extern const int updownsteps;
extern bool waffleOpen;

//Load Cell
extern HX711_ADC LoadCell;
extern const int calVal_eepromAdress;

//VALVE
void openValve() {digitalWrite(solenoidPin, HIGH);}
void closeValve() {digitalWrite(solenoidPin, LOW);}

//WAFFLE IRON
void openWaffleIron() {myStepper.step(updownsteps); waffleOpen = true;}
void closeWaffleIron() {myStepper.step(-updownsteps); waffleOpen = 0;}
bool waffleIronisOpen() {return waffleOpen;}

//ERROR
void SystemShutdown() {
    closeValve();
    closeWaffleIron();
    Serial.println("System Shutting Down");
}
float readForce() {
    static float filteredForce = 0.0f;
    const float alpha = 0.15f; // smoothing factor

    float reading = LoadCell.getData();

    if (isnan(reading)) {
        return filteredForce;
    }

    filteredForce = alpha * reading + (1.0f - alpha) * filteredForce;

    return filteredForce;
}

// CALIBRATION METHODS FOR HX711
void calibrate() {
    Serial.println("*** Calibration Start ***");
    Serial.println("Send 't' to tare.");

    while (true) {
        LoadCell.update();
        if (Serial.available()) {
            if (Serial.read() == 't') {
                LoadCell.tareNoDelay();
            }
        }
        if (LoadCell.getTareStatus()) {
            break;
        }
    }

    Serial.println("Place known mass and enter weight:");

    float known_mass = 0;
    while (known_mass == 0) {
        LoadCell.update();
        if (Serial.available()) {
            known_mass = Serial.parseFloat();
        }
    }

    LoadCell.refreshDataSet();
    float newCalFactor = LoadCell.getNewCalibration(known_mass);

    Serial.print("New Calibration Value: ");
    Serial.println(newCalFactor);

    EEPROM.put(calVal_eepromAdress, newCalFactor);
    Serial.println("Calibration saved.");

    LoadCell.setCalFactor(newCalFactor);
}

void changeSavedCalFactor() {
    float oldVal = LoadCell.getCalFactor();
    Serial.print("Current Cal Factor: ");
    Serial.println(oldVal);

    Serial.println("Enter new value:");

    float newVal = 0;
    while (newVal == 0) {
        if (Serial.available()) {
            newVal = Serial.parseFloat();
        }
    }

    LoadCell.setCalFactor(newVal);
    EEPROM.put(calVal_eepromAdress, newVal);

    Serial.println("New Cal Factor Saved.");
}
