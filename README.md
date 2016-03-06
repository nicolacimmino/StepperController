
A controller for stepper motors with a Serial USB interface.

The controller has the following registers controlling the way the motor is driven.

| Register  |         Name           | Description  |
| --------- | ---------------------- | -----------  |
| R0 | SPEED_RPM | Rotation speed in rounds/minute |
| R1 | ROTATION | Rotation direction. CW=0, CCW=1 |
| R2 | DRIVE_MODE | Drive mode. Half step = 1, Full Step = 1, Wave = 2 |
| R4 | HOLD_CTRL | Hold control power between steps. No = 0, Yes = 1 |

The controller has a USB Serial interface at 9600 BAUD, see table below for available commands.

| Command  |         Description           | Example | Example Response | 
| --------- | ---------------------- | ------ | ----- |
| Rn,v | Write register n with value v. | R0,20 | OK |
| rn | Read register n. | r0 | 20\nOK |
| G | Go, start rotation | G | OK |
| H | Halt, stop rotation | H | OK |
| S | Step, move motor one step. Requires R0>0 | S | OK |





