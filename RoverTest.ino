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

fgcu::RoverHead head{EchoPin, TriggerPin, ServoPin};

void setup() {

  delay(2000);

  Serial.begin(9600);

  head.turnHead(90);

  // blocking call to get the head turned 
  // to 90 degrees and distance taken
  bool done = false;
  do {
    done = head.run();
  } while (!done);
  
} // setup


void loop() {
  if (!wheels.run()) { // if last movement completed
    // The plan is to have the bot drive straight until it is 10 in from a wall.
    // Then it will look left -> right. 
    // Whichever is further is the direction it will turn to and go.

    // Get Left Dist
    delay(1000);
    Serial.println("Looking Left");
    head.turnHead(180);
    while(head.run()) {}
    word leftDist = head.getDistance();
    Serial.println(leftDist);

    // Get Right Dist
    delay(1000);
    Serial.println("Looking Straight");
    head.turnHead(90);
    while(head.run()) {}
    word straightDist = head.getDistance();
    Serial.println(straightDist);

    // Select Direction
    Serial.println("Choosing Direction");
    if (straightDist <= 10) {
      if (leftDist > 20) {
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
      wheels.turnLeft();
    }
    else if (moveDir == Right) {
      wheels.turnRight();
    }
    else {
      if (straightDist > 26) {
        wheels.moveForward(2.0f);
      }
      else {
        wheels.moveForward((float)(straightDist - 10) / 8.0f);
      }
    }
  }

} // loop