// StepperController implements a controller for a a bipolar 2-phases stepper motor.
//  Copyright (C) 2016 Nicola Cimmino
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see http://www.gnu.org/licenses/.
//

#define COILA_N 2
#define COILA_S 3
#define COILB_N 4
#define COILB_S 5

// Coil pulse stimulus direction.
// For a bipolar stepper motor we have two
// opposite energized positions and a neutral
#define PLUS 1
#define MINUS 2
#define NEUTRAL 3

// Rotation direction
#define CW 0
#define CCW 1

// Steps per round (20=18deg/step).
#define STEPS_PER_ROUND 20

void setup()
{
  pinMode(COILA_N, OUTPUT);
  pinMode(COILA_S, OUTPUT);
  pinMode(COILB_N, OUTPUT);
  pinMode(COILB_S, OUTPUT);  
}

void loop()
{ 
   stepMotorHalfStepDrive(CW, 2, false);
   // Any other code here as long as it doesn't take longer
   // than one step at the desired RPM.
}



/*
 * Move the motor one step forwad in the supplied 
 * direction. Using wave drive. There is really no
 * reason to prefer this to the full step, it just
 * gives less torque. It's here mainly for reference.
 */
void stepMotorWaveDrive(byte direction, uint8_t speedRPM, boolean holdControl)
{
    static uint8_t currentStep = 0;
    static long lastStepTime=0;
    
    // When spinning slowly we cannot afford small pulses to control
    // as they won't be enough to win the initial inertia. At higer speed
    // momentum will help so we can keep the pulse shorter, we also need to
    // keep the pulse shorter else we cannot achieved the desired RPM.
    uint8_t controlPulseDuration = (speedRPM<=20)?100:10;
    if(holdControl) controlPulseDuration=0;

    // This is the expected interval in mS between two steps to reach the
    // required RPM.
    long stepsInterval = ((STEPS_PER_ROUND*150.0f/speedRPM)-controlPulseDuration);
    
    // If it's not yet long enough since last step do nothing.
    if(millis()-lastStepTime<stepsInterval) return;
    
    lastStepTime = millis();

    // Move to the next step. This is a bipolar 2-phase motor
    // so when running in wave we have 4 different control
    // pulses.
    currentStep=currentStep+((direction==CCW)?-1:1);
    currentStep=currentStep%4;
    
    switch(currentStep)
    {
       case 0: driveMotor(NEUTRAL, PLUS, controlPulseDuration); break;
       case 1: driveMotor(PLUS, NEUTRAL, controlPulseDuration); break;
       case 2: driveMotor(NEUTRAL, MINUS, controlPulseDuration); break;
       case 3: driveMotor(MINUS, NEUTRAL, controlPulseDuration); break;  
    }
}


/*
 * Move the motor one step forwad in the supplied 
 * direction. Using full step drive.
 */
void stepMotorFullStepDrive(byte direction, uint8_t speedRPM, boolean holdControl)
{
    static uint8_t currentStep = 0;
    static long lastStepTime=0;
    
    // When spinning slowly we cannot afford small pulses to control
    // as they won't be enough to win the initial inertia. At higer speed
    // momentum will help so we can keep the pulse shorter, we also need to
    // keep the pulse shorter else we cannot achieved the desired RPM.
    uint8_t controlPulseDuration = (speedRPM<=20)?150:10;
    if(holdControl) controlPulseDuration=0;

    // This is the expected interval in mS between two steps to reach the
    // required RPM.
    long stepsInterval = ((STEPS_PER_ROUND*150.0f/speedRPM)-controlPulseDuration);
    
    // If it's not yet long enough since last step do nothing.
    if(millis()-lastStepTime<stepsInterval) return;
    
    lastStepTime = millis();

    // Move to the next step. This is a bipolar 2-phase motor
    // so when running full step we have 4 different control
    // pulses.
    currentStep=currentStep+((direction==CCW)?-1:1);
    currentStep=currentStep%4;
    
    switch(currentStep)
    {
       case 0: driveMotor(PLUS, PLUS, controlPulseDuration); break;
       case 1: driveMotor(PLUS, MINUS, controlPulseDuration); break;
       case 2: driveMotor(MINUS, MINUS, controlPulseDuration); break;
       case 3: driveMotor(MINUS, PLUS, controlPulseDuration); break;  
    }
}

/*
 * Move the motor one step forwad in the supplied 
 * direction. Using half step drive.
 */
void stepMotorHalfStepDrive(byte direction, uint8_t speedRPM, boolean holdControl)
{
    static uint8_t currentStep = 0;
    static long lastStepTime=0;
    
    // When spinning slowly we cannot afford small pulses to control
    // as they won't be enough to win the initial inertia. At higer speed
    // momentum will help so we can keep the pulse shorter, we also need to
    // keep the pulse shorter else we cannot achieved the desired RPM.
    uint8_t controlPulseDuration = (speedRPM<=20)?50:5;
    if(holdControl) controlPulseDuration=0;
    
    // This is the expected interval in mS between two steps to reach the
    // required RPM.
    long stepsInterval = ((STEPS_PER_ROUND*75.0f/speedRPM)-controlPulseDuration);

    // If it's not yet long enough since last step do nothing.
    if(millis()-lastStepTime<stepsInterval) return;
    
    lastStepTime = millis();
    
    // Move to the next step. This is a bipolar 2-phase motor
    // so when running half step we have 8 different control
    // pulses.
    currentStep=currentStep+((direction==CCW)?-1:1);
    currentStep=currentStep%8;
    
    switch(currentStep)
    {
       case 0: driveMotor(NEUTRAL, PLUS, controlPulseDuration); break;
       case 1: driveMotor(PLUS, PLUS, controlPulseDuration); break;
       case 2: driveMotor(PLUS, NEUTRAL, controlPulseDuration); break;
       case 3: driveMotor(PLUS, MINUS, controlPulseDuration); break;
       case 4: driveMotor(NEUTRAL, MINUS, controlPulseDuration); break;
       case 5: driveMotor(MINUS, MINUS, controlPulseDuration); break;
       case 6: driveMotor(MINUS, NEUTRAL, controlPulseDuration); break; 
       case 7: driveMotor(MINUS, PLUS, controlPulseDuration); break; 
       
    }
    
}

/*
 * Drives the motor sending the control pulse specified for
 * the supplied contol pulse duration (in mS). If the supplied
 * pulse duration is zero the control signal is left on indefinitely
 * so that hold torque can be taken advantage of.
 */
void driveMotor(byte coilA, byte coilB, uint8_t controlPulseDuration)
{
    digitalWrite(COILA_N, (coilA==PLUS)?HIGH:LOW);
    digitalWrite(COILA_S, (coilA==MINUS)?HIGH:LOW);
    digitalWrite(COILB_N, (coilB==PLUS)?HIGH:LOW);
    digitalWrite(COILB_S, (coilB==MINUS)?HIGH:LOW);
    if(coilA!=NEUTRAL && coilB!=NEUTRAL && controlPulseDuration!=0)
    {
      delay(controlPulseDuration);
      driveMotor(NEUTRAL,NEUTRAL, 0);
    }
}

