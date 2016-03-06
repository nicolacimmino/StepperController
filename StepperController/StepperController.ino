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

#include <avr/pgmspace.h>

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


const byte DriverSequence[] PROGMEM  = {
       // Half step drive
       NEUTRAL , PLUS,
       PLUS    , PLUS, 
       PLUS    , NEUTRAL, 
       PLUS    , MINUS,
       NEUTRAL , MINUS,
       MINUS   , MINUS, 
       MINUS   , NEUTRAL, 
       MINUS   , PLUS,  
       // Full step drive
       PLUS    , PLUS,
       PLUS    , PLUS,
       PLUS    , MINUS,
       PLUS    , MINUS,
       MINUS   , MINUS,
       MINUS   , MINUS,
       MINUS   , PLUS,
       MINUS   , PLUS,
       // Wave drive
       NEUTRAL , PLUS,
       NEUTRAL , PLUS,
       PLUS    , NEUTRAL,
       PLUS    , NEUTRAL,
       NEUTRAL , MINUS,
       NEUTRAL , MINUS,
       MINUS   , NEUTRAL,
       MINUS   , NEUTRAL
    };

#define DRIVE_HALF_STEP_OFFSET  0
#define DRIVE_FULL_STEP_OFFSET  8
#define DRIVE_WAVE_OFFSET  16

byte driveSequenceTableOffset = DRIVE_HALF_STEP_OFFSET;

// Rotation direction
#define CW 0
#define CCW 1

// Steps per round (20=18deg/step).
#define STEPS_PER_ROUND 20

// 50 Hz if board clock 16MHz
int timer1_counter = 64911; // 64286;

void setup()
{
  pinMode(COILA_N, OUTPUT);
  pinMode(COILA_S, OUTPUT);
  pinMode(COILB_N, OUTPUT);
  pinMode(COILB_S, OUTPUT);  

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = timer1_counter;   
  TCCR1B |= (1 << CS12);    // Set prescaler to 256 
  TIMSK1 |= (1 << TOIE1);   // Enable timer interrupt
  interrupts();             
}

ISR(TIMER1_OVF_vect)         
{
  TCNT1 = timer1_counter;   

  stepMotor(CW, 20, true);
   
}

void loop()
{
  driveSequenceTableOffset = DRIVE_HALF_STEP_OFFSET;
  //driveSequenceTableOffset = DRIVE_FULL_STEP_OFFSET;
  //driveSequenceTableOffset = DRIVE_WAVE_OFFSET;
}

/*
 * Move the motor one step forwad in the supplied 
 * direction. Using half step drive.
 */
void stepMotor(byte direction, uint8_t speedRPM, boolean holdControl)
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

    driveMotor(pgm_read_byte_near(driveSequenceTableOffset + DriverSequence + (currentStep*2)),
                pgm_read_byte_near(driveSequenceTableOffset + DriverSequence + (currentStep*2) + 1),
                controlPulseDuration);
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

