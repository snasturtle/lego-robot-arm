# LEGO Robotic Arm with Joystick & Potentiometer Control

A robotic arm built from LEGO Technic parts, driven by two DC motors (combined winch-style pull via string) and a servo-actuated gripper claw, controlled by an Arduino-compatible microcontroller.

## Overview

This project combines mechanical LEGO Technic construction with embedded electronics to build a functional robotic arm:
- **Two DC motors**, wired through an L293D H-bridge and combined onto a single control input, act as a winch, pulling a string to move the arm (gravity returns the arm when the string is let out).
- **A servo motor** drives a gear-driven LEGO claw mechanism to open and close a gripper, with the servo body anchored to the frame so only the drive gear rotates.
- **A joystick** provides proportional analog control over the winch motors.
- **A potentiometer** provides proportional control over the claw's open/closed position.

## Components Used

| Component | Purpose |

| SparkFun RedBoard (Arduino Uno-compatible) | Main microcontroller |
| L293D H-bridge IC | Dual motor driver for the two DC motors |
| 2x DC gear motors | Drive the winch string for arm movement |
| SG90 micro servo | Actuates the claw's gear train |
| Analog joystick module | Controls winch motor direction/speed |
| Potentiometer | Controls claw open/close position |
| Breadboard power supply module | Regulated 5V power from a 9V battery |
| 9V battery + clip | Power source |
| LEGO Technic pieces | Arm structure and claw mechanism |
| Breadboard + jumper wires | Circuit assembly |

## How It Works

**Arm movement:** The joystick's Y-axis drives both DC motors together, combining their pulling force on a single winch string. Pushing the stick forward spins both motors to reel in string; centering the stick stops them; pushing back reverses direction, letting the string out. Motor speed is proportional to how far the stick is pushed, with a deadzone near center to prevent drift. The two motors were originally wired to independent joystick axes for two-axis control, then combined onto one axis partway through the build once single-motor torque proved insufficient to lift the arm's load (see Debugging Notes).

**Claw control:** The potentiometer directly maps to a servo angle — turning the knob smoothly opens or closes the claw, rather than toggling between two fixed states. The claw's drive gear is mechanically coupled to the servo horn via a LEGO axle pressed through the horn's center mounting hole (coaxial with the servo's output spline), and the servo body itself is secured to the frame with a rubber band so reaction torque doesn't spin the servo case instead of the gear.

## Wiring

**L293D (16-pin DIP):**
| Pin | Function | Connects to |

| 1 | Enable 1,2 | D3 (PWM) |
| 2 | 1A | D4 |
| 3 | 1Y | Motor 1 terminal |
| 4, 5 | GND | Ground rail |
| 6 | 2Y | Motor 1 terminal |
| 7 | 2A | D5 |
| 8 | VCC2 (motor power) | 5V rail |
| 9 | Enable 3,4 | D6 (PWM) |
| 10 | 3A | D7 |
| 11 | 3Y | Motor 2 terminal |
| 12, 13 | GND | Ground rail |
| 14 | 4Y | Motor 2 terminal |
| 15 | 4A | D8 |
| 16 | VCC1 (logic power) | 5V rail |

**Other components:**
| Component | Pin | Connects to |
|---|---|---|
| Joystick | VCC / GND | 5V / GND |
| Joystick | VRx | A0 |
| Joystick | VRy | A1 |
| Potentiometer | Outer legs | 5V / GND |
| Potentiometer | Wiper | A2 |
| Servo | Signal | D11 |
| Servo | Power / GND | 5V / GND |

All grounds (RedBoard, L293D, power module, servo) share a common ground rail.

## Code

See ['robot_arm.ino'](./robot_arm.ino) for the full sketch. Core logic:

- 'mapJoystickToSpeed()' converts a raw joystick reading into a signed motor speed, with a deadzone to prevent drift near center.
- 'driveMotor()' sets direction and PWM speed on a given motor via the L293D.
- 'handleClawPot()' reads the potentiometer and proportionally maps it to a servo angle.

## Debugging Notes

The most significant issue encountered: both DC motors stopped responding entirely once the servo was attached in code, despite testing fine in isolation with hardcoded pin writes.

Root cause: on the ATmega328 (Uno/RedBoard), the 'Servo' library takes over Timer1 to generate its control pulses. Timer1 is also what drives 'analogWrite()' PWM output on pins D9 and D10 - the original enable pins for the two motors. The moment 'servo.attach()' was called, PWM on D9/D10 silently broke, even though the digital direction pins and chip wiring were otherwise correct.

Fix: moved both motor enable pins to D3 and D6, which use different timers and don't conflict with the Servo library.

This was isolated by:
1. Confirming both motors spun correctly when tested directly off the battery (ruled out the motors themselves).
2. Bypassing the RedBoard entirely and driving the L293D pins directly with jumper wires to 5V/GND (confirmed the chip and wiring were correct).
3. Running a minimal test sketch with hardcoded 'digitalWrite'/'analogWrite' calls and no 'Servo' library included - motors worked.
4. Adding the 'Servo' library and 'attach()' call back in - motors stopped working again, pointing directly at the timer conflict.

## Known Limitations

The winch mechanism's available torque, even with both motors combined onto a single string, was insufficient to reliably lift the arm's full assembled weight against gravity in the final build. The claw and its potentiometer-controlled servo are fully functional independent of this; the joystick-driven motor control and direction switching also work correctly, but the arm's lift capacity is the one subsystem that didn't reach its original design target within the project's timeframe.

Root cause: small hobby DC gear motors (rated for light loads) driving a direct-pull winch have limited torque at the string's effective radius; the combined pull of two motors improved this but didn't fully close the gap.

What would fix it with more time/parts: a geared reduction stage between the motor and the winch spool (trading speed for torque), or motors rated for a higher stall torque, would be the most direct fix. A smaller-diameter winch spool would also help, at the cost of slower winding speed.

## Possible Future Improvements

- Add a gear reduction stage or higher-torque motors to resolve the lift capacity limitation above
- Add limit switches to prevent the winch string from over-winding or fully unspooling
- 3D-printed servo-to-LEGO-axle adapter for a more robust claw connection than the current axle-through-drilled-horn approach
- Restore independent two-axis motor control (one motor per joint) once torque is no longer a shared constraint
- Position feedback (potentiometer or encoder) on the arm motor(s) for closed-loop control
- Wireless control via Bluetooth or a remote-control module in place of the wired joystick
