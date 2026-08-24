#include <Servo.h>

const int JOY_X = A0;
const int JOY_Y = A1;
const int POT_PIN = A2;

const int MOTOR1_IN1 = 4;
const int MOTOR1_IN2 = 5;
const int MOTOR1_EN  = 3;

const int MOTOR2_IN1 = 7;
const int MOTOR2_IN2 = 8;
const int MOTOR2_EN  = 6;

const int SERVO_PIN = 11;

const int JOY_CENTER = 512;
const int JOY_DEADZONE = 100;
const int MAX_SPEED = 80;

const int CLAW_OPEN_ANGLE = 30;
const int CLAW_CLOSED_ANGLE = 110;

Servo claw;

void setup() {
  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR1_EN, OUTPUT);

  pinMode(MOTOR2_IN1, OUTPUT);
  pinMode(MOTOR2_IN2, OUTPUT);
  pinMode(MOTOR2_EN, OUTPUT);

  claw.attach(SERVO_PIN);
  claw.write(CLAW_OPEN_ANGLE);

  Serial.begin(9600);
}

void loop() {
  int yVal = analogRead(JOY_Y);
  int ySpeed = mapJoystickToSpeed(yVal);

  // Both motors combined on one axis for extra pulling force on a single string.
  driveMotor(MOTOR1_IN1, MOTOR1_IN2, MOTOR1_EN, ySpeed);
  driveMotor(MOTOR2_IN1, MOTOR2_IN2, MOTOR2_EN, ySpeed);

  handleClawPot();

  delay(20);
}

int mapJoystickToSpeed(int rawVal) {
  int offset = rawVal - JOY_CENTER;
  if (abs(offset) < JOY_DEADZONE) {
    return 0;
  }
  int speed = map(offset, -JOY_CENTER, JOY_CENTER, -MAX_SPEED, MAX_SPEED);
  speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
  return speed;
}

void driveMotor(int in1, int in2, int enPin, int speed) {
  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(enPin, speed);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enPin, -speed);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enPin, 0);
  }
}

// Reads the potentiometer and moves the servo proportionally between CLAW_OPEN_ANGLE and CLAW_CLOSED_ANGLE based on knob position
void handleClawPot() {
  int potVal = analogRead(POT_PIN);
  int angle = map(potVal, 0, 1023, CLAW_OPEN_ANGLE, CLAW_CLOSED_ANGLE);
  claw.write(angle);
}
