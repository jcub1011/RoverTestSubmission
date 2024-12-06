// RoverTest.ino
// FGCU Computing & Software Engineering
// Prof. Allen
// Test the basic Rover robot interfaces. Uses a set of states
// to run the wheels through each of the basic movements and
// adjusts the head to point in the direction of movement. The
// Serial inteface is used to output direction of movement, along
// with direction of the head and distance measured.
// ------------------------------------------------------------

#include "RoverHead.h"
#include "RoverWheels.h"
#include "StopWatch.h"
//#include <math.h>

enum RoverMoveDir {
  Left,
  Right,
  Straight
};

fgcu::FourPin motorPinsLeft { A0, A1, A2, A3};
fgcu::FourPin motorPinsRight { 4, 5, 6, 7};
float speed = 600.f;
fgcu::RoverWheels wheels{motorPinsLeft, motorPinsRight, speed};

bool roverTurnLeft = false;
RoverMoveDir moveDir = Straight;

const byte ServoPin = 9;
const byte EchoPin = A5;
const byte TriggerPin = A4;

word currentLeftDistance = 0;
word prevLeftDistance = 0;

fgcu::RoverHead head{EchoPin, TriggerPin, ServoPin};

void setup() {

  delay(2000);

  Serial.begin(9600);

  // 180 is left, 90 is straight, and 0 is right.
  head.turnHead(90);
  while(head.run()) {}

  head.turnHead(180);
  while(head.run()) {}
  currentLeftDistance = head.getDistance();
  
} // setup


void loop() {
  if (!wheels.run()) { // if last movement completed
    // The plan is to have the bot drive straight until it is 10 in from a wall.
    // Then it will look left -> right. 
    // My kit is broken and cannot look right. I decided to make it check straight and forward instead.
    // It checks again immediately after turning in case it is at a dead end.

    // Get Left Dist
    delay(1000);
    Serial.println("Looking Left");
    head.turnHead(180);
    while(head.run()) {}
    prevLeftDistance = currentLeftDistance;
    currentLeftDistance = head.getDistance();
    Serial.println(prevLeftDistance);
    Serial.println(currentLeftDistance);

    // Get Straight Dist
    delay(1000);
    Serial.println("Looking Straight");
    head.turnHead(90);
    while(head.run()) {}
    word straightDist = head.getDistance();
    Serial.println(straightDist);

    // Select Direction
    Serial.println("Choosing Direction");
    if (straightDist <= 10) {
      if (straightDist <= 4) {
        // Back up if too close to wall.
          wheels.moveBackward(0.4);
          while (wheels.run()) {}
      }
      if (currentLeftDistance > 20) {
        moveDir = Left;
      }
      else {
        moveDir = Right;
      }
    }
    else {
      moveDir = Straight;
    }
    Serial.println(moveDir);

    // Perform Movement
    Serial.println("Performing Movement");
    if (moveDir == Left) {
      wheels.turnLeft(0.98f);
      prevLeftDistance = straightDist;
    }
    else if (moveDir == Right) {
      wheels.turnRight(1.02f);
      prevLeftDistance = straightDist;
    }
    else {
      // Going Straight
      float targetScale;
      if (straightDist > 26) {
        targetScale = 2.0f;

        int deltaDist = currentLeftDistance - prevLeftDistance;
        Serial.print("Current Distance Delta: ");
        Serial.println(deltaDist);
        if (currentLeftDistance < 12) {
          if (deltaDist <= 0) {
            // 10 degrees.
            wheels.turnRight(0.11);
            while (wheels.run()) {}
          }
          else if (currentLeftDistance < 6) {
            wheels.turnRight(0.11);
            while (wheels.run()) {}
          }
        }
        else if (currentLeftDistance > 14) {
          if (deltaDist >= 0) {
            // 10 degrees.
            wheels.turnLeft(0.11);
            while (wheels.run()) {}
          }
          else if (currentLeftDistance > 18) {
            wheels.turnLeft(0.11);
            while (wheels.run()) {}
          }
        }
      }
      else {
        targetScale = (float)(straightDist - 10) / 8.0f;
      }

      // Move half forward, then check for collision, correct, then move forwards again.
      wheels.moveForward(targetScale);
    }
  }

} // loop