// Function declarations and shared interfaces for the AutoWaffle project.

#ifndef MYLIBRARY_H
#define MYLIBRARY_H

// Hardware control helpers
void openValve();
void closeValve();

void openWaffleIron();
void closeWaffleIron();

void SystemShutdown();

bool waffleIronisOpen();

float readForce();

// Calibration helpers for HX711
void calibrate();
void changeSavedCalFactor();

#endif // MYLIBRARY_H

