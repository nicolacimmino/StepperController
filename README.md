
A controller for stepper motors with a Serial USB interface.

The controller has the following registers controlling the way the motor is driven.

| Register  |         Name           | Description  |
| --------- | ---------------------- | -----------  |
| R0 | SPEED_RPM | Rotation speed in rounds/minute |
| R1 | ROTATION | Rotation direction. CW=0, CCW=1 |
| R2 | DRIVE_MODE | Drive mode. Half step = 1, Full Step = 1, Wave = 2 |
| R4 | HOLD_CTRL | Hold control power between steps. No = 0, Yes = 1 |

