// https://learn.adafruit.com/adafruit-motor-shield/using-dc-motors
#include <AFMotor.h>

// MOTOR12_1KHZ is noisier but use less power
AF_DCMotor L_motor(2, MOTOR12_1KHZ);
AF_DCMotor R_motor(1, MOTOR12_1KHZ);

// Ultrasound trigger pin
const int triggerPin = A0;
// Ultrasound echo pin
const int echoPin = A1;

unsigned long prevTime = 0;
unsigned long timeWhitoutObstacle = 0;

/**
 * Returns distance in cm
 */
int checkDistance()
{
  // 1. Send ultrasound
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // 2. Reads the echoPin,
  // returns the sound wave travel time in microseconds
  int const duration = pulseIn(echoPin, HIGH);

  // 3. Calculating the distance
  // - 0.034 because speed of sound in air is 340 m/s
  // and duration is in μs and we want distance in cm
  // - Divided by two because sound waves are making round trips
  int const distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);
  return distance;
}

void setup()
{
  Serial.begin(9600);

  // Changes the following values to make the robot drive as straight as possible
  // 0 to 255
  L_motor.setSpeed(200);
  R_motor.setSpeed(200);

  // Turns motors off
  R_motor.run(RELEASE);
  L_motor.run(RELEASE);

  // Sets the ultrasound trigger pin as an output
  pinMode(triggerPin, OUTPUT);
  // Sets the ultrasound echo pin as an input
  pinMode(echoPin, INPUT);
}

void loop()
{
  const int distance = checkDistance();
  const unsigned long currentTime = millis();
  // Time elapsed in ms since last loop
  const unsigned long deltaTime = currentTime - prevTime;

  // Obstacle avoidance
  if (distance < 5)
  {
    timeWhitoutObstacle = 0;

    L_motor.setSpeed(150);
    R_motor.setSpeed(150);

    // Go backward
    R_motor.run(BACKWARD);
    L_motor.run(BACKWARD);

    delay(1000);

    // Turn
    R_motor.run(FORWARD);
    L_motor.run(BACKWARD);

    delay(500);
  }
  else
  {
    timeWhitoutObstacle += deltaTime;

    // Speed up when it's been 2 seconds without obstacle
    const int motorSpeed = timeWhitoutObstacle > 2000 ? 255 : 200;

    L_motor.setSpeed(motorSpeed);
    R_motor.setSpeed(motorSpeed);
    L_motor.run(FORWARD);
    R_motor.run(FORWARD);
  }

  prevTime = currentTime;
}
